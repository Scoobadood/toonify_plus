//
// Created by Dave Durbin (Old) on 17/7/21.
//

#include <QGL>
#include <QOpenGLDebug.h>
#include <spdlog/spdlog.h>
#include <QOpenGLTexture>
#include <gl3ext.h>
#include "toonify_widget.h"
#include "toonify_shaders.h"

void
debug_callback(const QOpenGLDebugMessage &msg) {
    spdlog::warn("debug: id: {} sev:{} type: {}  src: {}  msg: {}",
                 msg.id(), msg.severity(), msg.type(), msg.source(), msg.message().toStdString());
}

toonify_widget::toonify_widget(QWidget *parent, Qt::WindowFlags flags)
        : QOpenGLWidget(parent, flags) //
        , m_quad{
                // Vertex            Colour             Texture
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                -1.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                +1.0f, +1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
                +1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        } //
        , m_quad_indices{
                0, 1, 2,
                0, 2, 3} //
{
}

toonify_widget::~toonify_widget() {
    glDeleteTextures(1, &m_texture);
}

void check_shader_compilation(GLuint shader_id) {
    GLint compiled_ok;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compiled_ok);
    if (compiled_ok == GL_TRUE) {
        return;
    }
    GLint max_length = 0;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &max_length);
    std::vector<GLchar> info_log(max_length);
    glGetShaderInfoLog(shader_id, max_length, &max_length, &info_log[0]);
    const char *msg = (const char *) &info_log[0];
    spdlog::error(msg);
    throw std::runtime_error("Failed to compile shader");
}

void toonify_widget::initialise_program() {
    auto vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const auto *source = (const GLchar *) VERTEX_SHADER_SRC.c_str();
    glShaderSource(vertex_shader, 1, &source, nullptr);
    glCompileShader(vertex_shader);
    check_shader_compilation(vertex_shader);

    auto fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    source = (const GLchar *) DUMMY_FRAG_SHADER.c_str();
    glShaderSource(fragment_shader, 1, &source, nullptr);
    glCompileShader(fragment_shader);
    check_shader_compilation(fragment_shader);

    auto program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    glBindAttribLocation(program, 0, "a_texture_coord");
    glBindFragDataLocation(program, 0, "fragment_colour");

    glLinkProgram(program);
    GLint linked_ok = 0;
    glGetProgramiv(program, GL_LINK_STATUS, (int *) &linked_ok);
    if (linked_ok == GL_FALSE) {
        GLint max_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &max_length);

        // The max_length includes the NULL character
        std::vector<GLchar> info_log(max_length);
        glGetProgramInfoLog(program, max_length, &max_length, &info_log[0]);
        const char *msg = (const char *) &info_log[0];
        spdlog::error(msg);
        throw std::runtime_error("Link failed");
    }

    glDetachShader(program, vertex_shader);
    glDetachShader(program, fragment_shader);

    m_u_image_resolution = glGetUniformLocation(program, "u_image_resolution");
    m_u_input_image = glGetUniformLocation(program, "source_image");
    m_u_bin_width = glGetUniformLocation(program, "u_bin_width");
    m_u_psi_q = glGetUniformLocation(program, "u_psi_q");

    checkGLError("initialise_programs");
    m_program = program;
}

void
toonify_widget::initialise_buffers() {
    // Create vbo
    glGenBuffers(1, &m_vbo);

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_quad.size() * sizeof(float), m_quad.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_quad_indices.size() * sizeof(GLuint), m_quad_indices.data(),
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    checkGLError("initialise_buffers");
}

void
toonify_widget::set_image(const QImage &image) {
    m_source_image = image.convertToFormat(QImage::Format_RGBA8888);
    int width = m_source_image.width();
    int height = m_source_image.height();
    uchar *data = m_source_image.bits();
    glTexImage2D(GL_TEXTURE_2D, 0,
                 GL_RGB, width, height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    checkGLError("set image");

    // And now modify the texture coordinates
    float max_dim = width > height
                    ? (float) width
                    : (float) height;
    auto tex_width = (float) width / max_dim;
    auto tex_height = (float) height / max_dim;
    m_quad[6] = m_quad[14] = (1.0f - tex_width) * 0.5f;
    m_quad[7] = m_quad[31] = (1.0f + tex_height) * 0.5f;
    m_quad[22] = m_quad[30] = (1.0f + tex_width) * 0.5f;
    m_quad[23] = m_quad[15] = (1.0f - tex_height) * 0.5f;

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_quad.size() * sizeof(float), m_quad.data(), GL_STATIC_DRAW);
}

void
toonify_widget::initialise_textures() {
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    checkGLError("initialise_textures");

    QImage image{"test.png"};
    set_image(image);
}

void toonify_widget::initializeGL() {
    if (!initializeOpenGLFunctions()) {
        spdlog::error("Failed to initialise OpenGLFunctions");
        throw std::runtime_error("Failed to initialise OpenGLFunctions");
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    initialise_program();
    initialise_buffers();
    initialise_textures();

    create_render_target();
    checkGLError("initialiseGL");
}

void toonify_widget::resizeGL(int w, int h) {
    // Calculate aspect ratio
    float aspect = float(w) / float(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const float z_near = 0.1f, z_far = 100.0f, fov = 45.0f;

    // Reset projection
    m_projection.setToIdentity();

    // Set perspective projection
    m_projection.perspective(fov, aspect, z_near, z_far);
}

void
toonify_widget::checkGLError(const std::string &context) {
    auto err = glGetError();
    if (!err) return;
    spdlog::error("{}: {} ", context, err);
}

void
toonify_widget::create_render_target() {
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    GLuint FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

    // The texture we're going to render to
    GLuint renderedTexture;
    glGenTextures(1, &renderedTexture);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, renderedTexture);

    // Give an empty image to OpenGL ( the last "0" )
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 768, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    // Poor filtering. Needed !
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Set "renderedTexture" as our colour attachement #0
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

    // Set the list of draw buffers.
    GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
    // Always check that our framebuffer is ok
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        spdlog::error("Failed to create render target");
    }
}

void toonify_widget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    glUseProgram(m_program);
    glUniform1i(m_u_input_image, m_texture);
    glBindVertexArray(m_vao);

    glBindTexture(GL_TEXTURE_2D, m_texture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

//    glUniform2f(m_u_image_resolution, (float) m_source_image->width(), (float) m_source_image->height());
//    glUniform1i(m_u_input_image, m_texture);
//    glUniform1f(m_u_bin_width, 5);
//    glUniform1f(m_u_psi_q, 0.5f);
//    checkGLError("Bind program");
//
//    glEnableVertexAttribArray(0);

    checkGLError("drawElements");
}