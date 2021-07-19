#include <iostream>
#include <QSurfaceFormat>
#include <QApplication>
#include <QOpenGLContext>
#include <QScreen>
#include <spdlog/cfg/env.h>

#include "toonify_window.h"

int main(int argc, char **argv) {
    spdlog::cfg::load_env_levels();

    QSurfaceFormat format;
    format.setVersion(4, 1);
    format.setProfile(QSurfaceFormat::CoreProfile);
    if( format.testOption(QSurfaceFormat::DebugContext)) {
        format.setOption(QSurfaceFormat::DebugContext);
    } else {
        spdlog::warn( "No debug context available");
    }
    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);

    toonify_window window;
    window.show();
    return app.exec();
}
