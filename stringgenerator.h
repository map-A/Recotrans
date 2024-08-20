#pragma once
#include <QThread>
#include <atomic>
#include <memory>
#include <utility>

class Inference;
class Audio_async;
class StringGenerator : public QThread
{
    Q_OBJECT

public:
    explicit StringGenerator(QObject *parent = nullptr);
    ~StringGenerator() override;
    void run() override;
    void stop();
    void setModel(const QString &fileName);
    void setBuffer(std::shared_ptr<DoubleBuffer<float>> buffer)
    {
        inference->set_buffer(std::move(buffer));
    }
signals:
    void newStringAvailable(const QString &str);
private:
    std::atomic<bool> running{true};
    std::unique_ptr<Inference> inference;
};

class AudioCapture : public QThread
{
    Q_OBJECT

public:
    explicit AudioCapture(QObject *parent = nullptr)
        : QThread(parent)
    {
        audio = std::make_unique<Audio_async>(WHISPER_SAMPLE_RATE, 2000);
        if (!audio.get())
        {
            fprintf(stderr, "%s: audio.init() failed!\n", __func__);
        }
    }
    void setBuffer(std::shared_ptr<DoubleBuffer<float>> buffer)
    {
        audio->set_buffer(buffer);
    }
    ~AudioCapture() override
    {
        wait();
    }
protected:
    void run() override
    {
        audio->capture_audio(512);
    }
private:
    std::unique_ptr<Audio_async> audio;
};


