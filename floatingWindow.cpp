#include "floatingWindow.h"
#include "stream.h"
#include "stringgenerator.h"

FloatingWindow::FloatingWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Floating Window");
    resize(800, 300);
    centerAndMoveToBottom();
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);


    QVBoxLayout *layout = new QVBoxLayout(this);


    opacitySlider = new QSlider(Qt::Horizontal, this);
    opacitySlider->setRange(40, 100);
    opacitySlider->setValue(100);
    connect(opacitySlider, &QSlider::valueChanged, this, &FloatingWindow::changeOpacity);
    layout->addWidget(opacitySlider);

    label = new QLabel("Initial text", this);
    QFont font = label->font();
    font.setPointSize(24);
    label->setFont(font);
    layout->addWidget(label);
    setLayout(layout);
    createMenu();
}

FloatingWindow::~FloatingWindow()
{
    if (stringGenerator)
    {
        stringGenerator->stop();
        stringGenerator->wait();
    }
}

void FloatingWindow::centerAndMoveToBottom()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    int screenHeight = screen->geometry().height();
    int windowHeight = sizeHint().height();
    int yPos = screenHeight - windowHeight - 400;
    
    QRect availableGeometry = screen->availableGeometry();
    int xPos = (availableGeometry.width() - sizeHint().width()) / 2;
    
    move(xPos, yPos);
}

void FloatingWindow::updateLabel(const QString &text)
{
    label->setText(text);
}

void FloatingWindow::chooseColor()
{
    QColor color = QColorDialog::getColor(Qt::white, this, "选择背景颜色");
    if (color.isValid())
    {
        QPalette pal = palette();
        pal.setColor(QPalette::Background, color);
        setAutoFillBackground(true);
        setPalette(pal);
    }
}

void FloatingWindow::changeOpacity(int value)
{
    setWindowOpacity(static_cast<double>(value) / 100);
}

void FloatingWindow::showOpacitySlider()
{
    opacitySlider->setVisible(!opacitySlider->isVisible());
}

void FloatingWindow::openFile()
{
    QString fileName = QFileDialog::getOpenFileName(this, "选择文件", "");
    if (!fileName.isEmpty())
    {
        stringGenerator = std::make_unique<StringGenerator>(this);
        stringGenerator->setModel(fileName);
        connect(stringGenerator.get(), &StringGenerator::newStringAvailable, this, &FloatingWindow::updateLabel);
        stringGenerator->start();
    }
}

void FloatingWindow::createMenu()
{
    auto *menuBar = new QMenuBar(this);
    QMenu *settingsMenu = menuBar->addMenu("menu");

    auto *colorAction = new QAction("choose color", this);
    connect(colorAction, &QAction::triggered, this, &FloatingWindow::chooseColor);

    auto *opacityAction = new QAction("capacity", this);
    connect(opacityAction, &QAction::triggered, this, &FloatingWindow::showOpacitySlider);

    auto *openAction = new QAction("open file", this);
    connect(openAction, &QAction::triggered, this, &FloatingWindow::openFile);

    settingsMenu->addAction(colorAction);
    settingsMenu->addAction(opacityAction);
    settingsMenu->addAction(openAction);

    layout()->setMenuBar(menuBar);
}
