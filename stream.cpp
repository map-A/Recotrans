#include "stream.h"
#include <QDebug>
#include <vector>
#include <algorithm> // For std::all_of
#include <cmath>     // For std::fabs

std::vector<float> pcmf32;
std::vector<float> pcmf32_old;
std::vector<float> pcmf32_new;
std::vector<whisper_token> prompt_tokens;

const float EPSILON = 1e-6;

bool isSilent(const std::vector<float>& vector) {
    return std::all_of(vector.begin(), vector.end(),
                       [](float value) { return std::fabs(value) <= EPSILON; });
}

QString Inference::get_str() {
    QString ret;
    //if(isSilent(pcmf32)) return ret;
    whisper_full(ctx, wparams, pcmf32.data(), pcmf32.size());
        const int n_segments = whisper_full_n_segments(ctx);
        for (int i = 0; i < n_segments; ++i) {
            const char *text = whisper_full_get_segment_text(ctx, i);
            ret += text;
        }
    return ret;
}

void Inference::inference(int &n_iter) {
    whisper_full(ctx, wparams, pcmf32.data(), pcmf32.size());
    // print result;
    {
        printf("\r");
        const int n_segments = whisper_full_n_segments(ctx);
        for (int i = 0; i < n_segments; ++i) {
            const char *text = whisper_full_get_segment_text(ctx, i);
            if (params.no_timestamps) {
                printf("%s", text);
                fflush(stdout);
            } else {
                const int64_t t0 = whisper_full_get_segment_t0(ctx, i);
                const int64_t t1 = whisper_full_get_segment_t1(ctx, i);

                std::string output = text;

                if (whisper_full_get_segment_speaker_turn_next(ctx, i)) {
                    output += " [SPEAKER_TURN]";
                }

                output += "\n";

                printf("%s", output.c_str());
                fflush(stdout);

            }
        }
        printf("\n");
    }

    ++n_iter;

    if (!use_vad && (n_iter) == 0) {
        printf("\n");

        // keep part of the audio for next iteration to try to mitigate word boundary issues
        pcmf32_old = std::vector<float>(pcmf32.end() - n_samples_keep, pcmf32.end());

        // Add tokens of the last full length segment as the prompt
        if (!params.no_context) {
            prompt_tokens.clear();

            const int n_segments = whisper_full_n_segments(ctx);
            for (int i = 0; i < n_segments; ++i) {
                const int token_count = whisper_full_n_tokens(ctx, i);
                for (int j = 0; j < token_count; ++j) {
                    prompt_tokens.push_back(whisper_full_get_token_id(ctx, i, j));
                }
            }
        }
    }
    fflush(stdout);
}

void Inference::init_params(const QString &fileName) {
    params.model = fileName.toUtf8().constData();
    params.keep_ms = std::min(params.keep_ms, params.step_ms);
    params.length_ms = std::max(params.length_ms, params.step_ms);
    n_samples_step = (1e-3 * params.step_ms) * WHISPER_SAMPLE_RATE;
    n_samples_len = (1e-3 * params.length_ms) * WHISPER_SAMPLE_RATE;
    n_samples_keep = (1e-3 * params.keep_ms) * WHISPER_SAMPLE_RATE;

    use_vad = true; // sliding window mode uses VAD


    params.no_timestamps = !use_vad;
    params.no_context |= use_vad;
    params.max_tokens = 0;
    // whisper init
    if (params.language != "auto" && whisper_lang_id(params.language.c_str()) == -1) {
        fprintf(stderr, "error: unknown language '%s'\n", params.language.c_str());
        exit(0);
    }

    wparams = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    wparams.print_progress = false;
    wparams.print_special = false;
    wparams.print_realtime = false;
    wparams.no_context =true;
    wparams.print_timestamps = !params.no_timestamps;
    wparams.translate = params.translate;
    wparams.single_segment = !use_vad;
    wparams.max_tokens = params.max_tokens;
    wparams.language = params.language.c_str();
    wparams.n_threads = params.n_threads;
    wparams.audio_ctx = params.audio_ctx;
    wparams.tdrz_enable = params.tinydiarize; // [TDRZ]

    // disable temperature fallback
    //wparams.temperature_inc  = -1.0f;
    wparams.temperature_inc = params.no_fallback ? 0.0f : wparams.temperature_inc;

    wparams.prompt_tokens = params.no_context ? nullptr : prompt_tokens.data();
    wparams.prompt_n_tokens = params.no_context ? 0 : prompt_tokens.size();


}

void Inference::init_audio() {
    audio = new Audio_async(2000);
    if (!audio->init(WHISPER_SAMPLE_RATE, 512)) {
        fprintf(stderr, "%s: audio.init() failed!\n", __func__);
    }
}

void Inference::init_whisper() {
    const int n_samples_30s = (1e-3 * 20000.0) * WHISPER_SAMPLE_RATE;
    pcmf32.resize(n_samples_30s, 0.0f);
    pcmf32_new.resize(n_samples_30s, 0.0f);
    struct whisper_context_params cparams = whisper_context_default_params();
    cparams.use_gpu = params.use_gpu;
    cparams.flash_attn = params.flash_attn;
    ctx = whisper_init_from_file_with_params(params.model.c_str(), cparams);
}

Inference::Inference() {
    audio = nullptr;
    ctx = nullptr;
}

Inference::~Inference() {
    whisper_free(ctx);
}


