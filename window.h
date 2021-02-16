#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>

class Window : public QMainWindow
{
    Q_OBJECT
public:
    explicit Window(QWidget *parent = nullptr);
    void createNewWindow();

private:
    QList<QWidget*> windows;
signals:

};

#endif // WINDOW_H
