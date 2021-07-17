//
// Created by Dave Durbin (Old) on 17/7/21.
//

#ifndef TOONIFY_TOONIFY_WIDGET_H
#define TOONIFY_TOONIFY_WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>

class toonify_widget : public QOpenGLWidget {
public:
    explicit toonify_widget(QWidget *parent = nullptr,
                            Qt::WindowFlags flags = Qt::WindowFlags())
                            : QOpenGLWidget(parent, flags)
                            {}

protected:
    QMatrix4x4 m_projection;

    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;
};


#endif //TOONIFY_TOONIFY_WIDGET_H
