#include <iostream>
#include <QApplication>
#include <spdlog/cfg/env.h>

#include "toonify_window.h"

int main(int argc, char **argv) {
    spdlog::cfg::load_env_levels();

    QApplication app(argc, argv);

    toonify_window window;
    window.show();
    return app.exec();
}
