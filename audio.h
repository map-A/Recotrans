#pragma once
#include <atomic>
#include <cstdint>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <cstring>
#include "RtAudio.h"


#define FORMAT RTAUDIO_FLOAT32
#if defined(WIN32)
#include <windows.h>
#define SLEEP(milliseconds) Sleep((DWORD)milliseconds)
#else // Unix variants
#include <unistd.h>
#define SLEEP(milliseconds) usleep((unsigned long)(milliseconds * 1000.0))
#endif
template<typename T>
class DoubleBuffer
{
public:
    explicit DoubleBuffer(size_t size)
            : buffer_size(size), activeBuffer(&buffer1), inactiveBuffer(&buffer2),dataAvailable(false) {
        buffer1.resize(size);
        buffer2.resize(size);

    }
    void write(const uint8_t *data, size_t len)
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
    void read(std::vector<float> &data)
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
    std::vector<T> read()
    {
        std::unique_lock<std::mutex> lock(mutex);
        cv.wait(lock);
        return *inactiveBuffer;
    }
    void clear()
    {
        std::unique_lock<std::mutex> lock(mutex);
        activeBuffer->clear();
        inactiveBuffer->clear();
        buffer1.clear();
        buffer2.clear();
        dataAvailable = false;
        cv.notify_one();
    }

private:
    size_t buffer_size;
    std::vector<T> buffer1, buffer2;
    std::vector<T> *activeBuffer, *inactiveBuffer;
    bool dataAvailable;
    std::mutex mutex;
    std::condition_variable cv;
};

class Audio_async
{
public:
    explicit Audio_async(unsigned int sample_rate, int len_ms);
    ~Audio_async();

    void clear();

    void clean()
    {
        if (adc->isStreamOpen())
            adc->closeStream();
    }
    void capture_audio(unsigned int bufferFrames);

    void set_buffer(std::shared_ptr<DoubleBuffer<float>> buffer)
    {
        data = buffer;
    }

    void get(std::vector<float> &audio);

private:
    std::unique_ptr<RtAudio> adc;
    std::shared_ptr<DoubleBuffer<float>> data;
    RtAudio::StreamParameters oParams;
    int m_len_ms = 0;
    unsigned int m_sample_rate = 0;
};
