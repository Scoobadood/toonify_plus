//
// Created by Dave Durbin (Old) on 17/7/21.
//

#ifndef TOONIFY_TOONIFY_WIDGET_H
#define TOONIFY_TOONIFY_WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>

#include <vector>
#include <QOpenGLFunctions_4_1_Core>

class toonify_widget : public QOpenGLWidget, QOpenGLFunctions_4_1_Core //
{
public:
    explicit toonify_widget(QWidget *parent = nullptr,
                            Qt::WindowFlags flags = Qt::WindowFlags());

protected:
    QMatrix4x4 m_projection;

    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;

private:
    std::vector<QVector3D> m_quad;
    std::vector<GLuint> m_quad_indices;
};


#endif //TOONIFY_TOONIFY_WIDGET_H
