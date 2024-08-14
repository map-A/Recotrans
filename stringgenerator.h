#pragma once
#include <QThread>
#include <atomic>
#include <memory>

class Inference; 
class StringGenerator : public QThread {
    Q_OBJECT

public:
    explicit StringGenerator(QObject *parent = nullptr);
    ~StringGenerator() override; 
    void run() override;
    void setModel(const QString &fileName);
    void stop();

signals:
    void newStringAvailable(const QString &str);

private:
    std::atomic<bool> running{false}; 
    std::unique_ptr<Inference> in; 
};