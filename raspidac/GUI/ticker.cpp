#include "ticker.h"

Ticker::Ticker(QWidget *parent) : QWidget(parent),
    m_interval(20), m_pauseInterval(6000), m_textPosition(0), m_tickerText("")
{
    m_tickerTimer = new QTimer(this);
    m_pauseTimer = new QTimer(this);
    m_pauseTimer->setSingleShot(true);
    m_font = new QFont("Noto Sans", 14);
    m_fontMetrics = new QFontMetrics(*m_font);
    connect(m_tickerTimer, SIGNAL(timeout()), this, SLOT(updateTicker()));
    connect(m_pauseTimer, SIGNAL(timeout()), this, SLOT(restart()));

    m_tickerSpace = QString("            ");
    m_spaceWidth = m_fontMetrics->width(m_tickerSpace);
}

Ticker::~Ticker()
{

}

void Ticker::paintEvent(QPaintEvent *event)
{
    QPainter m_painter(this);
    m_painter.setFont(*m_font);
    m_painter.setPen(QColor("#fff"));
    m_painter.drawText(m_textPosition, 0, m_doubleTextWidth, 30, Qt::AlignVCenter, m_tickerText);
}

void Ticker::updateTicker()
{
    m_textPosition = m_textPosition - 1;
    if (m_textPosition < (-1) * (m_textWidth + m_spaceWidth))
    {
        stop();
        start();
    }
    update();
}

void Ticker::setText(QString text)
{
    m_textWidth = m_fontMetrics->width(text);
    m_tickerText = text + m_tickerSpace + text;
    m_doubleTextWidth = m_textWidth;
    m_textPosition = 0;
    m_tickerTimer->stop();
    update();
}

void Ticker::start()
{
    if (m_textWidth > width())
    {
        m_textPosition = 0;
        m_doubleTextWidth = 2 * m_textWidth + m_spaceWidth;
        m_pauseTimer->start(m_pauseInterval);
    }
}

void Ticker::stop()
{
    m_tickerTimer->stop();
    m_textPosition = 0;
    update();
}

void Ticker::restart()
{
    m_textPosition = 0;
    m_tickerTimer->start(m_interval);
}


