#pragma once
#include <QWidget>
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

class StringGenerator;

class FloatingWindow : public QWidget {
    Q_OBJECT

public:
    explicit FloatingWindow(QWidget *parent = nullptr);
    ~FloatingWindow();

private slots:
    void updateLabel(const QString &text);
    void chooseColor();
    void changeOpacity(int value);
    void showOpacitySlider();
    void openFile();
    void createMenu();

private:
    QLabel *label{nullptr};
    std::unique_ptr<StringGenerator> stringGenerator;
    QSlider *opacitySlider{nullptr};
};
