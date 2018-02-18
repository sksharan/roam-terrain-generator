#include "Utils.h"
#include <fstream>

noise::module::Perlin Utils::mod;

bool Utils::read_file(std::string filename, std::string& str) {
    std::ifstream stream;

    stream.open(filename);
    if (!stream.is_open()) {
        return false;
    }

    str.clear();

    stream.seekg(0, std::ios::end);
    str.resize(stream.tellg());
    stream.seekg(0, std::ios::beg);
    stream.read(&str[0], str.size());

    stream.close();

    return true;
}

GLuint Utils::create_shader(std::string shader_filename, GLenum shader_type) {
    GLuint shader;
    std::string shader_code;

    shader = glCreateShader(shader_type);
    if (shader == 0 || !Utils::read_file(shader_filename, shader_code)) {
        return 0;
    }

    const char* shader_code_cstr = shader_code.c_str();
    glShaderSource(shader, 1, (const GLchar**)&shader_code_cstr, NULL);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLint log_length;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        GLchar* info_log = (char*)malloc(log_length);
        glGetShaderInfoLog(shader, log_length, &log_length, info_log);
        fprintf(stderr, "Compile error in %s:\n%s", shader_filename.c_str(), info_log);
        free(info_log);
        return 0;
    }

    return shader;
}

GLuint Utils::create_program(GLuint shaders[], int num_shaders) {
    GLuint program = glCreateProgram();

    if (program == 0) {
        return 0;
    }

    for (int i = 0; i < num_shaders; i++) {
        glAttachShader(program, shaders[i]);
    }
    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        GLint log_length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        GLchar* info_log = (char*)malloc(log_length);
        glGetProgramInfoLog(program, log_length, &log_length, info_log);
        fprintf(stderr, "Problem with linking program:\n%s", info_log);
        free(info_log);
        return 0;
    }

    return program;
}

void Utils::print_GLerrors() {
    for (GLenum error = glGetError(); error != GL_NO_ERROR; error = glGetError()) {
        fprintf(stderr, "glGetError: %d\n", error);
    }
}

float Utils::get_value(float x, float y, float z) {
    const float factor = 2048.0f;
    const float amplitude = 265.0f;
    const float rand = 12.0f;
    return mod.GetValue((float)x / factor, rand, (float)z / factor) * amplitude;
}