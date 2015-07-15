#include "ticker.h"

Ticker::Ticker(QWidget *parent) : QWidget(parent),
    m_interval(20), m_textPosition(0)
{
    m_tickerText = new QString("");
    m_tickerTimer = new QTimer(this);
    m_font = new QFont("Helvetica", 24);
    m_fontMetrics = new QFontMetrics(*m_font);
    connect(m_tickerTimer, SIGNAL(timeout()), this, SLOT(updateTicker()));

//    m_tickerText = new QString("Das ist eine Laufschrift");
//    m_textWidth = fontMetrics.width(*m_tickerText);

//    m_textPosition = width();
//    m_tickerTimer->start(m_interval);
}

Ticker::~Ticker()
{

}

void Ticker::paintEvent(QPaintEvent *event)
{
    QPainter m_painter(this);
    m_painter.drawRect(0, 0, width()-1, height()-1);
    m_painter.setFont(*m_font);
    m_painter.drawText(m_textPosition, 0, 500, 30, Qt::AlignVCenter, *m_tickerText);
}

void Ticker::updateTicker()
{
    m_textPosition = m_textPosition - 1;
    if (m_textPosition < (-1) * m_textWidth)
    {
        m_textPosition = width();
    }
    update();
}

void Ticker::setText(QString text)
{
    *m_tickerText = text;
    m_textWidth = m_fontMetrics->width(*m_tickerText);
    m_textPosition = 0;
    m_tickerTimer->stop();
    update();
}

void Ticker::start()
{
    if (m_textWidth > width())
    {
        m_tickerTimer->start(m_interval);
    }
}

void Ticker::stop()
{
    m_tickerTimer->stop();
    m_textPosition = 0;
    update();
}



//void FontRunner::init()
//{
//    m_pTimerRunningText = new QTimer(this);
//    connect(m_pTimerRunningText,SIGNAL( timeout()),this,SLOT(updateFontRunner()));
//    m_RunningText = "Dies ist eine Laufschrift";
//    m_pTimerRunningText->start(10);
//    m_TextPosition = width();
//}


//timerfunction
//void FontRunner::updateFontRunner()
//{
//    m_TextPosition--;

//    //if the text out of the window move it on the right side...
//    if(m_TextPosition == (-1)*m_RunningText.length()*(m_Font.pointSize()))
//    {
//        m_TextPosition = width();
//    }
//    update();
//}

//void FontRunner::paintEvent(QPaintEvent* apo_event)
//{
//    QPainter o_painter(this);

//    o_painter.drawRect(0,0,width()-1,height()-1);

//    o_painter.setFont(m_Font);
//    o_painter.drawText(m_TextPosition,5,m_RunningText.length()*(m_Font.pointSize()),height()-10,Qt::AlignCenter,m_RunningText)
//}

