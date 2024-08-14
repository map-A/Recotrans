#include "stream.h"
#include <QDebug>
#include <vector>
#include <algorithm>
#include <cmath>


std::vector<float> pcmf32;
std::vector<float> pcmf32_old;
std::vector<float> pcmf32_new;
std::vector<whisper_token> prompt_tokens;

const float EPSILON = 1e-6;

// Utility function to check if a vector is silent (all values near zero)
bool isSilent(const std::vector<float>& vector) {
    return std::all_of(vector.begin(), vector.end(),
                       [](float value) { return std::fabs(value) <= EPSILON; });
}

// Get the transcription string from the current context
QString Inference::get_str() {
    QString ret;
    whisper_full(ctx, wparams, pcmf32.data(), pcmf32.size());

    int n_segments = whisper_full_n_segments(ctx);
    for (int i = 0; i < n_segments; ++i) {
        ret += whisper_full_get_segment_text(ctx, i);
    }
    return ret;
}

// Perform the inference and handle the output
void Inference::inference(int& n_iter) {
    whisper_full(ctx, wparams, pcmf32.data(), pcmf32.size());

    // Output the results
    const int n_segments = whisper_full_n_segments(ctx);
    for (int i = 0; i < n_segments; ++i) {
        const char* text = whisper_full_get_segment_text(ctx, i);

        if (params.no_timestamps) {
            printf("%s", text);
        } else {
            int64_t t0 = whisper_full_get_segment_t0(ctx, i);
            int64_t t1 = whisper_full_get_segment_t1(ctx, i);

            std::string output = text;

            if (whisper_full_get_segment_speaker_turn_next(ctx, i)) {
                output += " [SPEAKER_TURN]";
            }

            printf("%s\n", output.c_str());
        }
        fflush(stdout);
    }

    ++n_iter;

    if (!use_vad && n_iter == 0) {
        printf("\n");

        // Retain part of the audio for the next iteration to mitigate word boundary issues
        pcmf32_old.assign(pcmf32.end() - n_samples_keep, pcmf32.end());

        // Add tokens of the last full-length segment as the prompt
        if (!params.no_context) {
            prompt_tokens.clear();
            for (int i = 0; i < n_segments; ++i) {
                int token_count = whisper_full_n_tokens(ctx, i);
                for (int j = 0; j < token_count; ++j) {
                    prompt_tokens.push_back(whisper_full_get_token_id(ctx, i, j));
                }
            }
        }
    }
    fflush(stdout);
}

// Initialize parameters based on a configuration file (or default)
void Inference::init_params(const QString& fileName) {
    params.model = fileName.toUtf8().constData();
    params.keep_ms = std::min(params.keep_ms, params.step_ms);
    params.length_ms = std::max(params.length_ms, params.step_ms);

    n_samples_step = static_cast<int>(1e-3 * params.step_ms * WHISPER_SAMPLE_RATE);
    n_samples_len = static_cast<int>(1e-3 * params.length_ms * WHISPER_SAMPLE_RATE);
    n_samples_keep = static_cast<int>(1e-3 * params.keep_ms * WHISPER_SAMPLE_RATE);

    use_vad = true; // Sliding window mode uses VAD

    params.no_timestamps = !use_vad;
    params.no_context |= use_vad;
    params.max_tokens = 0;

    // Initialize Whisper
    if (params.language != "auto" && whisper_lang_id(params.language.c_str()) == -1) {
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
    wparams.prompt_tokens = params.no_context ? nullptr : prompt_tokens.data();
    wparams.prompt_n_tokens = params.no_context ? 0 : prompt_tokens.size();
}

// Initialize the audio system
void Inference::init_audio() {
    audio = new Audio_async(2000);
    if (!audio->init(WHISPER_SAMPLE_RATE, 512)) {
        fprintf(stderr, "%s: audio.init() failed!\n", __func__);
    }
}

// Initialize Whisper context and allocate memory
void Inference::init_whisper() {
    const int n_samples_30s = static_cast<int>(1e-3 * 20000.0 * WHISPER_SAMPLE_RATE);
    pcmf32.assign(n_samples_30s, 0.0f);
    pcmf32_new.assign(n_samples_30s, 0.0f);

    whisper_context_params cparams = whisper_context_default_params();
    cparams.use_gpu = params.use_gpu;
    cparams.flash_attn = params.flash_attn;

    ctx = whisper_init_from_file_with_params(params.model.c_str(), cparams);
}

// Constructor
Inference::Inference() : audio(nullptr), ctx(nullptr) {}

// Destructor
Inference::~Inference() {
    whisper_free(ctx);
    delete audio;
}
