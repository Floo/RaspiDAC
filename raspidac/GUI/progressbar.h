#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QWidget>
#include <QPainter>
#include <QRect>

class ProgressBar : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressBar(QWidget *parent = 0);
    ~ProgressBar();

signals:

public slots:
    void setMinimum(int);
    void setMaximum(int);
    void setValue(int);

protected:
    void paintEvent(QPaintEvent *event);

private:
    QRect *m_rect;
    int m_min;
    int m_max;
};

#endif // PROGRESSBAR_H
