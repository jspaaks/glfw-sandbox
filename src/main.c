#include "datenwolf/linmath.h"
#include <GLES3/gl32.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Vertex {
    vec2 pos;
    vec3 color;
} Vertex;

static void key_handler (GLFWwindow *, int, int, int, int);
void error_handler (int, const char *);
void show_usage (FILE *);
void show_glfw_version (void);

void error_handler (int error, const char * description) {
    (void) error;
    fprintf(stderr, "Error: %s\n", description);
}

static void key_handler (GLFWwindow * window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int main (int argc, char * argv[]) {

    if (argc == 2 && (strncmp(argv[1], "--help", 7) == 0 || strncmp(argv[1], "-h", 3) == 0)) {
        show_usage(stdout);
        exit(EXIT_SUCCESS);
    }

    if (argc != 1) {
        show_usage(stderr);
        exit(EXIT_FAILURE);
    }

    glfwSetErrorCallback(error_handler);
    if (glfwInit() != GLFW_TRUE) {
        fprintf(stderr, "Error initializing GLFW; aborting.\n");
        exit(EXIT_FAILURE);
    }
    show_glfw_version();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow * window = glfwCreateWindow(640, 480, "My Title", nullptr, nullptr);
    if (window == nullptr) {
        fprintf(stderr, "Error initializing window; aborting.\n");
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_handler);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    /* Set up some OpenGL related stuff here  */

    const Vertex vertices[3] = {
        {
         .pos = { -0.6f, -0.4f },
         .color = { 1.f, 0.f, 0.f },
         },
        {
         .pos = { 0.6f, -0.4f },
         .color = { 0.f, 1.f, 0.f },
         },
        {
         .pos = { 0.f, 0.6f },
         .color = { 0.f, 0.f, 1.f },
         },
    };

    const char * vertex_shader_text = "#version 330\n"
                                      "uniform mat4 MVP;\n"
                                      "in vec3 vCol;\n"
                                      "in vec2 vPos;\n"
                                      "out vec3 color;\n"
                                      "void main()\n"
                                      "{\n"
                                      "    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
                                      "    color = vCol;\n"
                                      "}\n";

    const char * fragment_shader_text = "#version 330\n"
                                        "in vec3 color;\n"
                                        "out vec4 fragment;\n"
                                        "void main()\n"
                                        "{\n"
                                        "    fragment = vec4(color, 1.0);\n"
                                        "}\n";

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, nullptr);
    glCompileShader(vertex_shader);

    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, nullptr);
    glCompileShader(fragment_shader);

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    const GLint mvp_location = glGetUniformLocation(program, "MVP");
    const GLint vpos_location = glGetAttribLocation(program, "vPos");
    const GLint vcol_location = glGetAttribLocation(program, "vCol");

    GLuint vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, pos));
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, color));

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        int width;
        int height;
        glfwGetFramebufferSize(window, &width, &height);
        const float ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        mat4x4 m;
        mat4x4 p;
        mat4x4 mvp;
        mat4x4_identity(m);
        mat4x4_rotate_Z(m, m, (float) glfwGetTime());
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat *) &mvp);
        glBindVertexArray(vertex_array);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

void show_glfw_version (void) {
    int major;
    int minor;
    int rev;
    glfwGetVersion(&major, &minor, &rev);
    fprintf(stdout, "Running GLFW version %d.%d.%d\n", major, minor, rev);
}

void show_usage (FILE * stream) {
    char * exename = "demo";
    fprintf(stream,
            "Usage: %s\n"
            "    GLFW tutorial with OpenGL.\n",
            exename);
}
