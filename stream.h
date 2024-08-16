#pragma once

#include "whisper.h"
#include "audio.h"
#include <QWidget>
#include <vector>
#include <string>
#include <thread>
#include <iostream>
#include <algorithm>
#include <memory>


constexpr float EPSILON = 1e-6;

//// Utility function to check if a vector is silent (all values near zero)
//bool isSilent(const std::vector<float>& vector) {
//    return std::all_of(vector.begin(), vector.end(),
//                       [](float value) { return std::fabs(value) <= EPSILON; });
//}

struct whisper_params {
    int32_t n_threads = std::min(4, static_cast<int32_t>(std::thread::hardware_concurrency()));
    int32_t step_ms = 2000;
    int32_t length_ms = 8000;
    int32_t keep_ms = 100;
    int32_t capture_id = -1;
    int32_t max_tokens = 32;
    int32_t audio_ctx = 0;

    float vad_thold = 0.8f;
    float freq_thold = 100.0f;

    bool detect_language = true;
    bool translate = false;
    bool no_fallback = true;
    bool print_special = false;
    bool no_context = true;
    bool no_timestamps = false;
    bool tinydiarize = false;
    bool use_gpu = true;
    bool flash_attn = true;

    std::string prompt = "Whisper, when transcribing speech to text, prioritize accurate punctuation. For clarity, make sure each sentence is contained in a single subtitle segment. If the sentence is too long, break it up at natural pauses in the speaker's speech to keep the subtitles concise and easy for the audience to understand. When blank speech is recognized, please output the empty string directly, dont output \"thank you\".";
    std::string language = "en";
    std::string model = "ggml-base.en.bin";
};


class Inference {
public:
    Inference();
    ~Inference();

    void init_params(const QString& fileName = QString());
    void init_audio();
    void init_whisper();
    QString get_str() const;

    whisper_params get_params() const { return params; }
    whisper_context* get_ctx() const { return ctx; }
    Audio_async* get_audio() const { return audio.get(); }

    std::vector<float> pcmf32;
    std::vector<float> pcmf32_old;
    std::vector<float> pcmf32_new;
    std::vector<whisper_token> prompt_tokens;

    int n_samples_step = 0;
    int n_samples_len = 0;
    int n_samples_keep = 0;
    bool use_vad = false;

private:
    whisper_params params;
    whisper_full_params wparams{};
    std::unique_ptr<Audio_async> audio; 
    whisper_context* ctx = nullptr;




};