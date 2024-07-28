#include "audio.h"

int input( void * /*outputBuffer*/, void *inputBuffer, unsigned int nBufferFrames,
           double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data )
{
    auto *iData = (InputData *) data;
    auto * stream  = (MY_TYPE *)inputBuffer;
    int len = nBufferFrames  * 2*2;

    size_t n_samples = len/sizeof(float);
    if (n_samples > iData->m_audio.size()) {
        n_samples = iData->m_audio.size();
        stream += (len - (n_samples * sizeof(float)));
    }
    {
        std::lock_guard<std::mutex> lock(iData->m_mutex);
        if (iData->m_audio_pos + n_samples > iData->m_audio.size()) {
            const size_t n0 = iData->m_audio.size() - iData->m_audio_pos;
            memcpy(&iData->m_audio[iData->m_audio_pos], stream, n0 * sizeof(float));
            memcpy(&iData->m_audio[0], stream + n0 * sizeof(float), (n_samples - n0) * sizeof(float));
            iData->m_audio_pos = (iData->m_audio_pos + n_samples) % iData->m_audio.size();
            iData->m_audio_len = iData->m_audio.size();
        } else {
            memcpy(&iData->m_audio[iData->m_audio_pos], stream, len);
            iData->m_audio_pos = (iData->m_audio_pos + n_samples) % iData->m_audio.size();
            iData->m_audio_len = std::min(iData->m_audio_len + n_samples, iData->m_audio.size());
        }
    }
    return 0;
}

void Audio_async::get(int ms, std::vector<float> & result) {
    if(!adc.isStreamRunning()){
        fprintf(stderr, "%s: not running!\n", __func__);
        return;
    }
    result.clear();
    {
        std::lock_guard<std::mutex> lock(data.m_mutex);
        if (ms <= 0) {
            ms = m_len_ms;
        }

        size_t n_samples = ( m_sample_rate* ms) / 1000;
        if (n_samples > data.m_audio_len) {
            n_samples = data.m_audio_len;
        }

        result.resize(n_samples);

        int s0 = data.m_audio_pos - n_samples;
        if (s0 < 0) {
            s0 += data.m_audio.size();
        }

        if (s0 + n_samples > data.m_audio.size()) {
            const size_t n0 = data.m_audio.size() - s0;
            memcpy(result.data(), &data.m_audio[s0], n0 * sizeof(float));
            memcpy(&result[n0], &data.m_audio[0], (n_samples - n0) * sizeof(float));
        } else {
            memcpy(result.data(), &data.m_audio[s0], n_samples * sizeof(float));
        }
    }
}


Audio_async::Audio_async(int len_ms) {
    m_len_ms = len_ms;
    adc.showWarnings( true );
    oParams.nChannels = 1;
    oParams.firstChannel = 0;
    oParams.deviceId = adc.getDefaultOutputDevice();
    oParams.is_output=false;

}

Audio_async::~Audio_async() = default;

bool Audio_async::init(unsigned int sample_rate,unsigned int bufferFrames) {
    m_sample_rate = sample_rate;
    data.m_audio.resize((sample_rate*m_len_ms*3)/1000);
    if ( adc.openStream( &oParams, nullptr, FORMAT,
                         sample_rate, &bufferFrames, &input, (void *)&data ) ){
        clean();
    }
    if ( adc.startStream() ){
        clean();
    }

    return true;
}



bool Audio_async::clear() {
    {
        std::lock_guard<std::mutex> lock(data.m_mutex);
        data.m_audio_len = 0;
        data.m_audio_len = 0;
    }
    return true;
}



