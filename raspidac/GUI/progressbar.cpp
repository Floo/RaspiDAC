#include "progressbar.h"

ProgressBar::ProgressBar(QWidget *parent) : QWidget(parent),
    m_min(0), m_max(0)
{
    m_rect = new QRect(0, 0, height(), 0);
    show();
}

ProgressBar::~ProgressBar()
{
    delete m_rect;
}

void ProgressBar::setMaximum(int max)
{
    m_max = max;
}

void ProgressBar::setMinimum(int min)
{
    m_min = min;
}

void ProgressBar::setValue(int value)
{
    int w = 0;
    if (m_max -m_min == 0)
        w = 0;
    else
        w = (int)((float)width() * ((float)value / (float)(m_max - m_min)));
    m_rect->setWidth(w);
    m_rect->setHeight(height());
    update();
}

void ProgressBar::paintEvent(QPaintEvent*)
{
    QPainter m_painter(this);
    QColor m_color = QColor("#e0e0e0");
    QColor m_bgcolor = QColor("#8d8d8d");
    QRect m_bgrect = QRect(0, 0, width(), height());
    m_painter.setPen(m_color);
    m_painter.fillRect(m_bgrect, m_bgcolor);
    m_painter.fillRect(*m_rect, m_color);
}
