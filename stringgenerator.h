
#pragma once

#include <QThread>

class StringGenerator : public QThread {
Q_OBJECT

public:
    explicit StringGenerator(QObject *parent = nullptr);
    void run() override;
    void setModel(const QString &fileName);
    void stop();

signals:
    void newStringAvailable(const QString &str);

private:
    bool running;
    Inference *in;
};
