#include "audio.h"

int input(void * /*outputBuffer*/, void *inputBuffer, unsigned int nBufferFrames,
          double /*streamTime*/, RtAudioStreamStatus /*status*/, void *data)
{
    auto *stream = static_cast<uint8_t *>(inputBuffer);
    auto *doubleBuffer = static_cast<DoubleBuffer<float> *>(data);
    int len = nBufferFrames * 2 * 2;
    doubleBuffer->write(stream, len);
    return 0;
}

Audio_async::Audio_async(unsigned int sample_rate, int len_ms) : adc(std::make_unique<RtAudio>()), m_len_ms(len_ms), m_sample_rate(sample_rate)
{
}
void Audio_async::capture_audio(unsigned int bufferFrames)
{
    adc->showWarnings(true);
    oParams.nChannels = 1;
    oParams.firstChannel = 0;
    oParams.deviceId = adc->getDefaultOutputDevice();
    oParams.is_output = false;
    if (adc->openStream(&oParams, nullptr, FORMAT, m_sample_rate, &bufferFrames, &input, (void *)&data))
    {
        clean();
    }
    if (adc->startStream())
    {
        clean();
    }
}

Audio_async::~Audio_async() = default;

void Audio_async::clear()
{
    data->clear();
}

void Audio_async::get(std::vector<float> &result)
{
    if (!adc->isStreamRunning())
    {
        fprintf(stderr, "%s: not running!\n", __func__);
        return;
    }
    data->read(result);
}
