//
// Created by Dave Durbin (Old) on 17/7/21.
//

#ifndef TOONIFY_TOONIFY_WIDGET_H
#define TOONIFY_TOONIFY_WIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLDebugLogger>
#include <QOpenGLTexture>
#include <QMatrix4x4>
#include <QOpenGLFunctions_4_1_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

#include <vector>

class toonify_widget : public QOpenGLWidget, QOpenGLFunctions_4_1_Core //
{
public:
    explicit toonify_widget(QWidget *parent = nullptr,
                            Qt::WindowFlags flags = Qt::WindowFlags());

    ~toonify_widget() override;


protected:
    QMatrix4x4 m_projection;

    void initializeGL() override;

    void resizeGL(int w, int h) override;

    void paintGL() override;

private:
    void checkGLError(const std::string &context);
    void initialise_program();
    void initialise_textures();
    void initialise_buffers();

    std::vector<float> m_quad;
    std::vector<GLuint> m_quad_indices;

    GLuint m_program;

    GLuint m_texture;
    QImage m_source_image;

    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
    GLint m_u_image_resolution;
    GLint m_u_input_image;
    GLint m_u_bin_width;
    GLint m_u_psi_q;
};


#endif //TOONIFY_TOONIFY_WIDGET_H
