#include "audio.h"

template <typename T>
void DoubleBuffer<T>::write(const uint8_t *data, size_t len)
{
    if (len > buffer_size)
    {
        throw std::runtime_error("Data size exceeds buffer size.");
    }
    std::unique_lock<std::mutex> lock(mutex);
    std::copy(data, data + len, activeBuffer->begin());
    dataAvailable = true;
    cv.notify_one();
}

template <typename T>
void DoubleBuffer<T>::read(std::vector<float> &data)
{
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this]
            { return dataAvailable; });
    data.clear();
    data.resize(inactiveBuffer->size() / sizeof(float));
    std::copy(inactiveBuffer->begin(), inactiveBuffer->end(), data.data());
    std::swap(activeBuffer, inactiveBuffer);
    dataAvailable = false;
}

template <typename T>
std::vector<T> DoubleBuffer<T>::read()
{
    std::unique_lock<std::mutex> lock(mtx);
    data_ready.wait(lock);
    return *inactive_buffer;
}

template <typename T>
void DoubleBuffer<T>::clear()
{
    std::unique_lock<std::mutex> lock(mutex);
    activeBuffer->clear();
    inactiveBuffer->clear();
    buffer1.clear();
    buffer2.clear();
    dataAvailable = false;
    cv.notify_one();
}

// 将inputBuffer中的数据存入data中
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
