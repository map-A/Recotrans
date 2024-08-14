#include "LyricWidget.h"
#include <QPainter>
#include <QFontMetrics>
#include "stringgenerator.h"

LyricWidget::LyricWidget(QWidget *parent)
        : QWidget(parent), m_currentIndex(0), m_yOffset(0)
{
    setWindowTitle("Recotrans");
    resize(1600, 400);
    centerAndMoveToBottom();
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);


    auto *layout = new QVBoxLayout(this);
    opacitySlider = new QSlider(Qt::Horizontal, this);
    opacitySlider->setRange(40, 100);
    opacitySlider->setValue(100);
    connect(opacitySlider, &QSlider::valueChanged, this, &LyricWidget::changeOpacity);
    layout->addWidget(opacitySlider);
    setLayout(layout);
    createMenu();
    m_currentIndex = 0;
    m_yOffset = height() / 2;
    m_timer.start(100);
}

void LyricWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 20));

    if (m_lyrics.isEmpty())
        return;

    QFontMetrics fm(painter.font());
    int lineHeight = fm.height();

    int y = m_yOffset;
    for (int i = m_currentIndex; i < m_lyrics.size(); ++i)
    {
        painter.drawText(10, y, m_lyrics[i]);
        y += lineHeight;
        if (y > height())
            break;
    }
}
void LyricWidget::updateLyrics(const QString &text) {
    m_lyrics.push_back(text);
    if(m_lyrics.size()>5){
        m_lyrics.pop_front();
    }
    m_yOffset -= 2;
    if (m_yOffset + QFontMetrics(QFont("Arial", 20)).height() < 0)
    {
        m_yOffset = height() / 2;
        m_currentIndex++;
        if (m_currentIndex >= m_lyrics.size())
        {
            m_currentIndex = 0;
        }
    }
    update();
}
LyricWidget::~LyricWidget() {
    if (stringGenerator)
    {
        stringGenerator->stop();
        stringGenerator->wait();
    }
}

void LyricWidget::centerAndMoveToBottom() {
    QScreen *screen = QGuiApplication::primaryScreen();
    int screenHeight = screen->geometry().height();
    int windowHeight = sizeHint().height();
    int yPos = screenHeight - windowHeight - 400;

    QRect availableGeometry = screen->availableGeometry();
    int xPos = (availableGeometry.width() - sizeHint().width()) / 2;
    move(xPos, yPos);
}

void LyricWidget::chooseColor() {
    QColor color = QColorDialog::getColor(Qt::white, this, "选择背景颜色");
    if (color.isValid())
    {
        QPalette pal = palette();
        pal.setColor(QPalette::Background, color);
        setAutoFillBackground(true);
        setPalette(pal);
    }
}

void LyricWidget::changeOpacity(int value) {
    setWindowOpacity(static_cast<double>(value) / 100);
}

void LyricWidget::showOpacitySlider() {
    opacitySlider->setVisible(!opacitySlider->isVisible());
}

void LyricWidget::openFile() {
    QString fileName = QFileDialog::getOpenFileName(this, "选择文件", "");
    if (!fileName.isEmpty())
    {
        stringGenerator = std::make_unique<StringGenerator>(this);
        stringGenerator->setModel(fileName);
        connect(stringGenerator.get(), &StringGenerator::newStringAvailable, this, &LyricWidget::updateLyrics);
        stringGenerator->start();
    }
}

void LyricWidget::createMenu() {
    auto *menuBar = new QMenuBar(this);
    QMenu *settingsMenu = menuBar->addMenu("menu");

    auto *colorAction = new QAction("choose color", this);
    connect(colorAction, &QAction::triggered, this, &LyricWidget::chooseColor);

    auto *opacityAction = new QAction("capacity", this);
    connect(opacityAction, &QAction::triggered, this, &LyricWidget::showOpacitySlider);

    auto *openAction = new QAction("open file", this);
    connect(openAction, &QAction::triggered, this, &LyricWidget::openFile);

    settingsMenu->addAction(colorAction);
    settingsMenu->addAction(opacityAction);
    settingsMenu->addAction(openAction);

    layout()->setMenuBar(menuBar);
}

