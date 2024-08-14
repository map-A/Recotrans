// LyricWidget.h
#ifndef LYRICWIDGET_H
#define LYRICWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QStringList>
#include <QSlider>
#include <QLabel>
#include <QColorDialog>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QPushButton>
#include <QDebug>
#include <memory>
#include <QTimer>
#include <QDialog>
#include "QtGui/qscreen.h"
#include "stream.h"
#include <QApplication>

class StringGenerator;

class LyricWidget : public QWidget
{
Q_OBJECT

public:
    explicit LyricWidget(QWidget *parent = nullptr);
    void setLyrics(const QStringList &lyrics);

    void centerAndMoveToBottom();

    ~LyricWidget();


protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateLyrics(const QString &text);
    void chooseColor();
    void changeOpacity(int value);
    void showOpacitySlider();
    void openFile();
    void createMenu();


private:
    QStringList m_lyrics;
    int m_currentIndex;
    int m_yOffset;
    QTimer m_timer;
    std::unique_ptr<StringGenerator> stringGenerator;
    QSlider *opacitySlider{nullptr};
};

#endif // LYRICWIDGET_H