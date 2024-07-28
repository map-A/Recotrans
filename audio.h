#pragma once
#include <atomic>
#include <cstdint>
#include <vector>
#include <mutex>
#include "RtAudio.h"


typedef uint8_t MY_TYPE;
#define FORMAT RTAUDIO_FLOAT32
#if defined( WIN32 )
#include <windows.h>
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds )
#else // Unix variants
#include <unistd.h>
  #define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

struct InputData {
    std::mutex       m_mutex;
    std::vector<float> m_audio;
    size_t             m_audio_pos = 0;
    size_t             m_audio_len = 0;
};

class Audio_async {
public:
    explicit Audio_async(int len_ms);
    ~Audio_async();

    bool init(unsigned int sample_rate,unsigned int bufferFrames);

    bool clear();

    void clean(){
        if ( adc.isStreamOpen() ) adc.closeStream();
    }

    void get(int ms, std::vector<float> &audio);

private:
    RtAudio adc;
    InputData data{};
    RtAudio::StreamParameters oParams;
    int m_len_ms = 0;
    unsigned int m_sample_rate = 0;
};
