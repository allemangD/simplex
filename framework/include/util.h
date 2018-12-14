#include <utility>

#include <utility>

#ifndef GL_TEMPLATE_UTIL_H
#define GL_TEMPLATE_UTIL_H

#include "resource.h"

#include <glad/glad.h>

#include <string>
#include <vector>

namespace util {
    template<typename T>
    void bufferData(GLenum target, std::vector<T> data, GLenum usage) {
        glBufferData(target, data.size() * sizeof(T), &data.front(), usage);
    }

    template<typename T>
    void bufferData(GLenum target, T &data, GLenum usage) {
        glBufferData(target, sizeof(T), &data, usage);
    }

    void shaderFiles(GLuint shader, std::vector<std::string> &paths) {
        std::vector<std::string> strs;
        std::vector<const char *> c_strs;

        for (const auto &path : paths) strs.push_back(readFile(path));
        for (const auto &str:strs) c_strs.push_back(str.c_str());

        glShaderSource(shader, (GLsizei) c_strs.size(), &c_strs.front(), nullptr);
    }

    std::string shaderInfoLog(GLuint shader) {
        GLint log_len;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
        char log[log_len];
        glGetShaderInfoLog(shader, log_len, nullptr, log);
        return std::string(log);
    }

    std::string programInfoLog(GLuint program) {
        GLint log_len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
        char log[log_len];
        glGetProgramInfoLog(program, log_len, nullptr, log);
        return std::string(log);
    }

    GLuint buildShader(GLenum kind, const std::string &name, std::vector<std::string> paths) {
        GLuint shader = glCreateShader(kind);
        shaderFiles(shader, paths);

        glCompileShader(shader);

        GLint comp;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &comp);
        if (!comp) {
            std::string log = shaderInfoLog(shader);
            fprintf(stderr, "SHADER ERROR (%s):\n%s", name.c_str(), log.c_str());
            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    GLuint buildShader(GLenum kind, const std::vector<std::string> &paths) {
        switch (kind) {
        case GL_VERTEX_SHADER:
            return buildShader(kind, "VERTEX", paths);
        case GL_FRAGMENT_SHADER:
            return buildShader(kind, "FRAGMENT", paths);
        default:
            return buildShader(kind, "?", paths);
        }
    }

    GLuint buildShader(const std::string &path) {
        std::string ext = path.substr(path.rfind('.'));
        std::string name;

        std::vector<std::string> paths{path};

        if (ext == ".vert") {
            return buildShader(GL_VERTEX_SHADER, paths);
        } else if (ext == ".frag") {
            return buildShader(GL_FRAGMENT_SHADER, paths);
        } else {
            fprintf(stderr, "Cannot parse path %s\n", path.c_str());
            return 0;
        }
    }

    GLuint buildProgram(bool separable, std::vector<GLuint> shaders) {
        GLuint program = glCreateProgram();

        if (separable)
            glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);

        for (GLuint shader : shaders)
            glAttachShader(program, shader);

        glLinkProgram(program);

        GLint link;
        glGetProgramiv(program, GL_LINK_STATUS, &link);
        if (!link) {
            std::string log = programInfoLog(program);
            fprintf(stderr, "PROGRAM ERROR:\n%s", log.c_str());
            glDeleteProgram(program);
            return 0;
        }

        return program;
    }
}

#endif //GL_TEMPLATE_UTIL_H
