#include "ticker.h"

Ticker::Ticker(QWidget *parent) : QWidget(parent),
    m_length(100), m_interval(20), m_pauseInterval(6000),
    m_textPosition(0),
    m_fontSize(14), m_tickerText(""), m_isRunning(false), m_singleRun(false)
{
    m_tickerTimer = new QTimer(this);
    m_pauseTimer = new QTimer(this);
    m_pauseTimer->setSingleShot(true);
    m_font = new QFont("Noto Sans", m_fontSize);
    m_fontMetrics = new QFontMetrics(*m_font);
    connect(m_tickerTimer, SIGNAL(timeout()), this, SLOT(updateTicker()));
    connect(m_pauseTimer, SIGNAL(timeout()), this, SLOT(restart()));

    m_tickerSpace = QString("            ");
    m_spaceWidth = m_fontMetrics->width(m_tickerSpace);
}

Ticker::~Ticker()
{

}

void Ticker::setFontSize(int fs, bool bold)
{
    m_fontSize = fs;
    delete m_font;
    delete m_fontMetrics;
    m_font = new QFont("Noto Sans", m_fontSize);
    m_font->setBold(bold);
    m_fontMetrics = new QFontMetrics(*m_font);
    m_spaceWidth = m_fontMetrics->width(m_tickerSpace);
}

bool Ticker::isRunning()
{
    return m_isRunning;
}

void Ticker::setLength(int len)
{
    m_length = len;
}

void Ticker::setPos(int x, int y)
{
    m_xpos = x;
    m_ypos = y;
}

void Ticker::setSingleRun(bool single)
{
    m_singleRun = single;
}

void Ticker::paintEvent(QPaintEvent*)
{
    QPainter m_painter(this);
    m_painter.setFont(*m_font);
    m_painter.setPen(QColor("#fff"));
    m_painter.drawText(m_textPosition, 0, m_doubleTextWidth, m_textHeight, Qt::AlignVCenter, m_tickerText);
}

void Ticker::updateTicker()
{
    m_textPosition = m_textPosition - 1;
    if (m_textPosition < (-1) * (m_textWidth + m_spaceWidth))
    {
        stop();
        emit tickerStopped();
        if (!m_singleRun)
        {
            start();
        }
    }
    update();
}

void Ticker::setText(QString text)
{
    m_textWidth = m_fontMetrics->width(text);
    m_textHeight = m_fontMetrics->height();
    m_tickerText = text + m_tickerSpace + text;
    m_doubleTextWidth = m_textWidth;
    setGeometry(0, 0, m_length, m_textHeight);
    move(m_xpos, m_ypos);
    stop();
}

void Ticker::start()
{
    if (m_textWidth > width())
    {
        m_textPosition = 0;
        m_doubleTextWidth = 2 * m_textWidth + m_spaceWidth;
        m_pauseTimer->start(m_pauseInterval);
        m_isRunning = true;
    }
}

void Ticker::stop()
{
    m_tickerTimer->stop();
    m_textPosition = 0;
    m_isRunning = false;
    update();
}

void Ticker::restart()
{
    m_textPosition = 0;
    m_tickerTimer->start(m_interval);
}


