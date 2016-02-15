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
    void stop();

signals:

private slots:
    void updateTicker();
    void restart();

public slots:
    void start();

protected:
    void paintEvent(QPaintEvent *event);

private:
    QTimer *m_tickerTimer;
    QTimer *m_pauseTimer;
    QFont *m_font;
    QFontMetrics *m_fontMetrics;
    int m_interval;
    int m_pauseInterval;
    int m_textPosition;
    int m_textWidth;
    QString m_tickerText;

    QString m_tickerSpace;
    int m_spaceWidth;
    int m_doubleTextWidth;


};

#endif // TICKER_H
