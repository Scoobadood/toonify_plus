//
// Created by Dave Durbin (Old) on 17/7/21.
//

#include <QGL>
#include <QOpenGLDebug.h>
#include <spdlog/spdlog.h>
#include "toonify_widget.h"

void
debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message,
              const void *userParam) {
    spdlog::info("GL CALLBACK: {} type = 0x{:x}, severity = 0x{:x}, message = {}",
             ( type == QOpenGLDebugMessage::Type::ErrorType ? "** GL ERROR **" : "" ),
             type, severity, message );
}


toonify_widget::toonify_widget(QWidget *parent, Qt::WindowFlags flags)
        : QOpenGLWidget(parent, flags) //
        , m_quad{
                {-0.5f, -0.5f, 0.0f},
                {0.0f,  0.0f,  0.0f},
                {-0.5f, +0.5f, 0.0f},
                {0.0f,  1.0f,  0.0f},
                {+0.5f, -0.5f, 0.0f},
                {1.0f,  0.0f,  0.0f},
                {+0.5f, +0.5f, 0.0f},
                {1.0f,  1.0f,  0.0f},
        } //
        , m_quad_indices{
                0, 1, 2,
                1, 2, 3} //
{
}

void toonify_widget::initializeGL() {
//    glEnable(GL_AMD_debug_output);
    QSurfaceFormat format =  QSurfaceFormat::defaultFormat();
    auto maj = format.majorVersion();
    auto min = format.minorVersion();
    initializeOpenGLFunctions();
//    glDebugMessageCallback(debugCallback, nullptr);

//    program = genProgram("../06_HelloTexture/vert.glsl", "../06_HelloTexture/frag.glsl");
//    glUseProgram(program);

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_quad.size() * sizeof(QVector3D), &m_quad[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_quad_indices.size() * sizeof(GLuint), &m_quad_indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(QVector3D), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(QVector3D), (void *) sizeof(QVector3D));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    QImage image("../lightbulb-solid.svg");
    // QImage image("../test.png");
    qDebug() << image;
    // this outputs
    // QImage(QSize(352, 512),format=6,depth=32,devicePixelRatio=1,bytesPerLine=1408,sizeInBytes=720896)

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 image.constBits());
    // glGenerateMipmap(GL_TEXTURE_2D);

//    lightbulbSolidLocation = glGetUniformLocation(program, "lightbulbSolid");
//    glUniform1i(lightbulbSolidLocation, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void toonify_widget::resizeGL(int w, int h) {
    // Update projection matrix and other size related settings:
    m_projection.setToIdentity();
    m_projection.perspective(45.0f, w / float(h), 0.01f, 100.0f);
}

void toonify_widget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Render the quad.

}