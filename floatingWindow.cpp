#include "floatingWindow.h"
#include "stream.h"
#include "stringgenerator.h"




FloatingWindow::FloatingWindow(QWidget *parent)
        : QWidget(parent) {
    setWindowTitle("Floating Window");

    QScreen *screen = QGuiApplication::primaryScreen();
    int screenHeight = screen->geometry().height();
    int windowHeight = sizeHint().height();
    int yPos = screenHeight - windowHeight-400;
    QRect availableGeometry = screen->availableGeometry();
    int xPos = (availableGeometry.width() - sizeHint().width()) / 2;

    resize(800, 300);
    move(xPos, yPos);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    QVBoxLayout *layout = new QVBoxLayout(this);


    opacitySlider = new QSlider(Qt::Horizontal, this);
    opacitySlider->setRange(40, 100);
    opacitySlider->setValue(100);
    opacitySlider->setVisible(opacitySlider->isVisible());
    connect(opacitySlider, &QSlider::valueChanged, this, &FloatingWindow::changeOpacity);
    layout->addWidget(opacitySlider);

    setLayout(layout);
    setWindowTitle("window translate");
    createMenu();

    label = new QLabel("Initial text", this);

    QFont font = label->font();
    font.setPointSize(24);
    label->setFont(font);

    layout->addWidget(label);

}

FloatingWindow::~FloatingWindow() {
    stringGenerator->stop();
    stringGenerator->wait();
}

void FloatingWindow::updateLabel(const QString &text) {
    label->setText(text);
}


void FloatingWindow::createMenu() {
    auto *menuBar = new QMenuBar(this);
    QMenu *settingsMenu = menuBar->addMenu("menu");

    auto *colorAction = new QAction("choose color", this);
    connect(colorAction, &QAction::triggered, this, &FloatingWindow::chooseColor);

    auto *opacityAction = new QAction("Opacity", this);
    connect(opacityAction, &QAction::triggered, this, &FloatingWindow::showOpacitySlider);

    auto *openAction = new QAction("openfile", this);
    connect(openAction, &QAction::triggered, this, &FloatingWindow::openFile);

    settingsMenu->addAction(colorAction);
    settingsMenu->addAction(opacityAction);
    settingsMenu->addAction(openAction);

    layout()->setMenuBar(menuBar);
}

