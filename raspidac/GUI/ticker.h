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
    void setFontSize(int fontSize, bool bold = false);
    bool isRunning();
    void setLength(int length);
    void setPos(int x, int y);
    void setSingleRun(bool single);

signals:
    void tickerStopped();

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
    int m_length;
    int m_interval;
    int m_pauseInterval;
    int m_textPosition;
    int m_textWidth;
    int m_textHeight;
    int m_fontSize;
    QString m_tickerText;
    int m_xpos;
    int m_ypos;

    QString m_tickerSpace;
    int m_spaceWidth;
    int m_doubleTextWidth;
    bool m_isRunning;
    bool m_singleRun;
};

#endif // TICKER_H
