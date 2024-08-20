#pragma once

#include "inference.h"
#include "audio.h"
#include "stringgenerator.h"
#include <QDebug>
#include <thread>
#include <chrono>
#include <iostream>

StringGenerator::StringGenerator(QObject *parent)
    : QThread(parent)
{
    inference = std::make_unique<Inference>();
    if (!inference.get())
    {
        fprintf(stderr, "%s: inference.init() failed!\n", __func__);
    }
}

void StringGenerator::run()
{          
    while (running)
    {   
        generatedString = inference->process();
        emit newStringAvailable(generatedString);
    }
}

StringGenerator::~StringGenerator()
{
    stop();
    wait();
}

void StringGenerator::stop()
{
    running = false;
}
void StringGenerator::setModel(const QString &fileName)
{
    if (inference.get())
    {
        inference->init_params(fileName);
        inference->init_whisper();
    }
}