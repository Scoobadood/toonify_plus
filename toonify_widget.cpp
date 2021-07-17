//
// Created by Dave Durbin (Old) on 17/7/21.
//

#include "toonify_widget.h"

void toonify_widget::initializeGL() {
    // Set up the rendering context, load shaders and other resources, etc.:
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void toonify_widget::resizeGL(int w, int h) {
    // Update projection matrix and other size related settings:
    m_projection.setToIdentity();
    m_projection.perspective(45.0f, w / float(h), 0.01f, 100.0f);
}

void toonify_widget::paintGL() {
    // Draw the scene:
    glClear(GL_COLOR_BUFFER_BIT);
}