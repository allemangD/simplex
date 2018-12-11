#ifndef GL_TEMPLATE_UTIL_H
#define GL_TEMPLATE_UTIL_H

#include "resource.h"

#include <glad/glad.h>

#include <initializer_list>
#include <string>

namespace util {
    void shaderFile(GLuint shader, const std::string &path) {
        const std::string str = readFile(path);
        const char *c_str = str.c_str();
        glShaderSource(shader, 1, &c_str, nullptr);
    }

    const std::string shaderInfoLog(GLuint shader) {
        GLint log_len;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
        char log[log_len];
        glGetShaderInfoLog(shader, log_len, nullptr, log);
        return std::string(log);
    }

    const std::string programInfoLog(GLuint program) {
        GLint log_len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
        char log[log_len];
        glGetProgramInfoLog(program, log_len, nullptr, log);
        return std::string(log);
    }

    GLuint buildShader(const std::string &path) {
        std::string ext = path.substr(path.rfind("."));

        GLenum kind;
        if (ext == ".vert") {
            kind = GL_VERTEX_SHADER;
        } else if (ext == ".frag") {
            kind = GL_FRAGMENT_SHADER;
        } else {
            fprintf(stderr, "Cannot parse path %s\n", path.c_str());
            return 0;
        }

        GLuint shader = glCreateShader(kind);
        util::shaderFile(shader, path);
        glCompileShader(shader);

        GLint comp;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &comp);
        if (!comp) {
            const std::string log = shaderInfoLog(shader);
            fprintf(stderr, "SHADER ERROR: %s\n%s", path.c_str(), log.c_str());
            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    GLuint buildProgram(std::initializer_list<GLuint> shaders) {
        GLuint program = glCreateProgram();

        for (GLuint shader : shaders)
            glAttachShader(program, shader);

        glLinkProgram(program);

        GLint link;
        glGetProgramiv(program, GL_LINK_STATUS, &link);
        if (!link) {
            const std::string log = programInfoLog(program);
            fprintf(stderr, "PROGRAM ERROR:\n%s", log.c_str());
            glDeleteProgram(program);
            return 0;
        }

        return program;
    }
}

#endif //GL_TEMPLATE_UTIL_H
