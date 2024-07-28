#pragma once

#include "stream.h"
#include "stringgenerator.h"
#include "QDebug"
StringGenerator::StringGenerator(QObject *parent)
        : QThread(parent), running(true) {
    in = new Inference();

}

void StringGenerator::run() {
    auto params = in->get_params();
    auto ctx = in->get_ctx();
    auto audio = in->get_audio();

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
            audio->get(params.step_ms, pcmf32_new);
            if ((int) pcmf32_new.size() > 2 * in->n_samples_step) {
                fprintf(stderr, "\n\n%s: WARNING: cannot process audio fast enough, dropping audio ...\n\n", __func__);
                audio->clear();
                continue;
            }
            if ((int) pcmf32_new.size() >= in->n_samples_step) {
                audio->clear();
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        const int n_samples_new = pcmf32_new.size();
        const int n_samples_take = std::min((int) pcmf32_old.size(),
                                            std::max(0, in->n_samples_keep + in->n_samples_len - n_samples_new));
        pcmf32.resize(n_samples_new + n_samples_take);
        for (int i = 0; i < n_samples_take; i++) {
            pcmf32[i] = pcmf32_old[pcmf32_old.size() - n_samples_take + i];
        }
        memcpy(pcmf32.data() + n_samples_take, pcmf32_new.data(), n_samples_new * sizeof(float));
        pcmf32_old = pcmf32;
        auto newStr = in->get_str();
        qDebug()<<newStr;
        std::cout<<std::endl;
        emit newStringAvailable(newStr);
    }
}

void StringGenerator::stop() {
    running = false;
}

void StringGenerator::setModel(const QString &fileName) {
    in->init_params(fileName);
    in->init_audio();
    in->init_whisper();
}