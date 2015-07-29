#ifndef TICKER_H
#define TICKER_H

#include <QWidget>
#include <QPainter>
#include <QTimer>

class Ticker : public QWidget
{
    Q_OBJECT
public:
    explicit Ticker(QWidget *parent = 0);
    ~Ticker();
    void setText(QString);
    void start();
    void stop();

signals:

private slots:
    void updateTicker();

public slots:

protected:
    void paintEvent(QPaintEvent *event);

private:
    QTimer *m_tickerTimer;
    QString *m_tickerText;
    QFont *m_font;
    QFontMetrics *m_fontMetrics;
    int m_interval;
    int m_textPosition;
    int m_textWidth;
};

#endif // TICKER_H
