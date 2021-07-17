#ifndef TOONIFY_WINDOW_H
#define TOONIFY_WINDOW_H

#include <QMainWindow>

namespace Ui {
class toonify_window;
}

class toonify_window : public QMainWindow
{
    Q_OBJECT

public:
    explicit toonify_window(QWidget *parent = nullptr);
    ~toonify_window();

private:
    Ui::toonify_window *ui;
};

#endif // TOONIFY_WINDOW_H
