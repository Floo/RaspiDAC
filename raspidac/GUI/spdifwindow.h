#ifndef SPDIFWINDOW_H
#define SPDIFWINDOW_H

#include <QWidget>
#include <QTime>

namespace Ui {
class SpdifWindow;
}

class SpdifWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SpdifWindow(QWidget *parent = 0);
    ~SpdifWindow();
    void setInput(QString);
    void setInputName(QString);

public slots:
    void setCurrentTime();

private:
    Ui::SpdifWindow *ui;
};

#endif // SPDIFWINDOW_H
