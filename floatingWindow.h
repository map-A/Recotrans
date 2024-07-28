#pragma once
#include <QVBoxLayout>
#include <QTimer>
#include <QDialog>
#include "QtGui/qscreen.h"
#include "stream.h"
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QColorDialog>
#include <QSlider>
#include <QWidget>
#include <QLabel>
#include <QThread>
#include <QMenuBar>
#include <QMenu>
#include <QFileDialog>
#include <QDebug>
#include "stringgenerator.h"

class FloatingWindow : public QWidget {
Q_OBJECT

public:
    FloatingWindow(QWidget *parent = nullptr);
    ~FloatingWindow();

public slots:

private slots:
    void updateLabel(const QString &text);
    void chooseColor() {
        QColor color = QColorDialog::getColor(Qt::white, this, "选择背景颜色");
        if (color.isValid()) {
            QPalette pal = palette();
            pal.setColor(QPalette::Background, color);
            setAutoFillBackground(true);
            setPalette(pal);
        }
    }

    void changeOpacity(int value) {
        setWindowOpacity((double)value/100);
    }
    void showOpacitySlider() {
        opacitySlider->setVisible(!opacitySlider->isVisible());
    }

    void openFile()
    {
        QString fileName = QFileDialog::getOpenFileName(this,"choose file","");
        stringGenerator = new StringGenerator(this);
        stringGenerator->setModel(fileName);
        connect(stringGenerator, &StringGenerator::newStringAvailable, this, &FloatingWindow::updateLabel);
        stringGenerator->start();
    }



    void createMenu();

signals:


private:
    QLabel *label;
    StringGenerator *stringGenerator{};
    QSlider *opacitySlider;
};
