#pragma once

#include "stream.h"
#include "stringgenerator.h"
#include <QDebug>
#include <thread>
#include <chrono>
#include <iostream>

StringGenerator::StringGenerator(QObject *parent)
    : QThread(parent), in(std::make_unique<Inference>()) {}

void StringGenerator::run() {
    running = true;
    whisper_params params = in->get_params();
    auto* ctx = in->get_ctx();
    auto* audio = in->get_audio();

    if (!whisper_is_multilingual(ctx)) {
        if (params.language != "en" || params.translate) {
            params.language = "en";
            params.translate = false;
            fprintf(stderr, "%s: WARNING: model is not multilingual, ignoring language and translation options\n",
                    __func__);
        }
    }

    while (running) {
        while (true) {
            audio->get(params.step_ms, in->pcmf32_new);

            if (in->pcmf32_new.size() > 2 * in->n_samples_step) {
                fprintf(stderr, "\n\n%s: WARNING: cannot process audio fast enough, dropping audio ...\n\n", __func__);
                audio->clear();
                continue;
            }

            if (in->pcmf32_new.size() >= in->n_samples_step) {
                audio->clear();
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        const int n_samples_new = in->pcmf32_new.size();
        const int n_samples_take = std::min(static_cast<int>(in->pcmf32_old.size()),
                                            std::max(0, in->n_samples_keep + in->n_samples_len - n_samples_new));
        in->pcmf32.resize(n_samples_new + n_samples_take);
        std::copy(in->pcmf32_old.end() - n_samples_take, in->pcmf32_old.end(), in->pcmf32.begin());
        std::copy(in->pcmf32_new.begin(), in->pcmf32_new.end(), in->pcmf32.begin() + n_samples_take);

        in->pcmf32_old = in->pcmf32;

        auto generatedString = in->get_str();
        qDebug() << generatedString;
        std::cout << std::endl;
        emit newStringAvailable(generatedString);
    }
}

StringGenerator::~StringGenerator() {
    stop();
    wait();
}

void StringGenerator::stop() {
    running = false;
}
void StringGenerator::setModel(const QString &fileName) {
    if (in) {
        in->init_params(fileName);
        in->init_audio();
        in->init_whisper();
    }
}