#pragma once
#include "whisper.h"
#include <cstdio>
#include <string>
#include <thread>
#include <vector>
#include <iostream>
#include <QWidget>
#include "audio.h"


extern std::vector<float> pcmf32;
extern std::vector<float> pcmf32_old;
extern std::vector<float> pcmf32_new;
extern std::vector<whisper_token> prompt_tokens;
bool isSilent(const std::vector<float>& vector);
struct whisper_params {
    int32_t n_threads  = std::min(4, (int32_t) std::thread::hardware_concurrency());
    int32_t step_ms    = 2000;
    int32_t length_ms  = 8000;
    int32_t keep_ms    = 100;
    int32_t capture_id = -1;
    int32_t max_tokens = 32;
    int32_t audio_ctx  = 0;

    float vad_thold    = 0.8f;
    float freq_thold   = 100.0f;

    bool detect_language = true;

    bool translate     = false;
    bool no_fallback   = false;
    bool print_special = false;
    bool no_context    = true;
    bool no_timestamps = false;
    bool tinydiarize   = false;
    bool use_gpu       = true;
    bool flash_attn    = true;
    std::string prompt = "When no content is recognized, please do not output any content.";
    std::string language  = "en";
    std::string model     = "ggml-base.en.bin";
};

static bool whisper_params_parse(int argc, char ** argv, whisper_params &params);

class Inference{
    ~Inference();

public:
    void inference(int &n_iter);
    void init_params(const QString &fileName=nullptr);
    void init_audio();
    void init_whisper();
    QString get_str();
    whisper_params get_params(){
        return params;
    }
    whisper_context * get_ctx(){
        return ctx;
    }
    Audio_async *get_audio(){
        return audio;
    }

    int n_samples_step{} ;
    int n_samples_len{};
    int n_samples_keep{};

    Inference();

private:
    whisper_params params;
    whisper_full_params wparams{};
    Audio_async *audio;
    struct whisper_context * ctx;
    bool use_vad{};
};