#include "inference.h"
#include <QDebug>
#include <algorithm>
#include <cmath>

Inference::Inference() : ctx(nullptr) {}

Inference::~Inference()
{
    whisper_free(ctx);
}

void Inference::init_params(const QString &fileName)
{
    if (!fileName.isEmpty())
    {
        params.model = fileName.toUtf8().constData();
    }
    params.keep_ms = std::min(params.keep_ms, params.step_ms);
    params.length_ms = std::max(params.length_ms, params.step_ms);

    n_samples_step = static_cast<int>(1e-3 * params.step_ms * WHISPER_SAMPLE_RATE);
    n_samples_len = static_cast<int>(1e-3 * params.length_ms * WHISPER_SAMPLE_RATE);
    n_samples_keep = static_cast<int>(1e-3 * params.keep_ms * WHISPER_SAMPLE_RATE);

    use_vad = true;

    params.no_timestamps = !use_vad;
    params.no_context |= use_vad;
    params.max_tokens = 0;

    if (params.language != "auto" && whisper_lang_id(params.language.c_str()) == -1)
    {
        fprintf(stderr, "error: unknown language '%s'\n", params.language.c_str());
        exit(EXIT_FAILURE);
    }

    wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    wparams.print_progress = false;
    wparams.print_special = false;
    wparams.print_realtime = false;
    wparams.no_context = true;
    wparams.print_timestamps = !params.no_timestamps;
    wparams.translate = params.translate;
    wparams.single_segment = !use_vad;
    wparams.max_tokens = params.max_tokens;
    wparams.language = params.language.c_str();
    wparams.n_threads = params.n_threads;
    wparams.audio_ctx = params.audio_ctx;
    wparams.tdrz_enable = params.tinydiarize;

    wparams.temperature_inc = params.no_fallback ? 0.0f : wparams.temperature_inc;
}

// Initialize Whisper context and allocate memory
void Inference::init_whisper()
{
    whisper_context_params cparams = whisper_context_default_params();
    cparams.use_gpu = params.use_gpu;
    cparams.flash_attn = params.flash_attn;
    ctx = whisper_init_from_file_with_params(params.model.c_str(), cparams);

     if (!whisper_is_multilingual(ctx))
    {
        if (params.language != "en" || params.translate)
        {
            params.language = "en";
            params.translate = false;
            fprintf(stderr, "%s: WARNING: model is not multilingual, ignoring language and translation options\n",
                    __func__);
        }
    }
}


QString Inference::process()
{
    inference_str.clear();
    auto pcmf32 = data->read();
    if (pcmf32.empty())
    {  
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        return inference_str;
    }
    whisper_full(ctx, wparams, pcmf32.data(), pcmf32.size());

    int n_segments = whisper_full_n_segments(ctx);
    for (int i = 0; i < n_segments; ++i)
    {
        inference_str += whisper_full_get_segment_text(ctx, i);
    }
    return inference_str;
}

QString Inference::get_str(std::vector<float>&pcmf32) const
{
    QString ret;
    whisper_full(ctx, wparams, pcmf32.data(), pcmf32.size());

    int n_segments = whisper_full_n_segments(ctx);
    for (int i = 0; i < n_segments; ++i)
    {
        ret += whisper_full_get_segment_text(ctx, i);
    }
    return ret;
}