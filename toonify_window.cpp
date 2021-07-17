#include "toonify_window.h"
#include "ui_toonify_window.h"

toonify_window::toonify_window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::toonify_window)
{
    ui->setupUi(this);
}

toonify_window::~toonify_window()
{
    delete ui;
}
