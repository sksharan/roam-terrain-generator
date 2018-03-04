#ifndef TG_UTILS_H
#define TG_UTILS_H

#include "Configuration.h"
#include <GL/glew.h>
#include <noise.h>
#include <string>

/* Class providing various utility functions. */
class Utils {
public:
    /* Reads the contents of 'filename' into 'str' and returns true iff successful. */
    static bool read_file(std::string filename, std::string& str);

    /* Create and compile a shader of type 'shader_type' with code from 'shader_filename'.
    Returns the GLuint associated with that shader on success. Prints error
    message and returns 0 on failure. */
    static GLuint create_shader(std::string shader_filename, GLenum shader_type);

    /* Create a program from the shaders in 'shaders' (which has length 'num_shaders').
    Returns the GLuint asscoiated with the program on success. Prints error message and
    returns 0 on failure.*/
    static GLuint create_program(GLuint shaders[], int num_shaders);

    /* Prints all errors reported by glGetError() to stderr. */
    static void print_GLerrors();

    /* Returns a value from a Perlin noise function given x, y, z values. */
    static float get_value(float x, float y, float z, Configuration& configuration);

private:
    static noise::module::Perlin mod; //Module used to get height values
};

#endif