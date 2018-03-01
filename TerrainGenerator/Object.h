#ifndef TG_OBJECT_H
#define TG_OBJECT_H

#include <vector>
#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

/*  An Object that can be rendered to the screen. 

    To create an Object, first create a new object:
        Object obj;

    Create the VAO for the Object:
        obj.create_vao();

    Then specify the vertices, texcoords, and normals for the Object:
        obj.set_vertices(vertices, GL_STATIC_DRAW);
        obj.set_texcoords(texcoords, GL_STATIC_DRAW);
        obj.set_normals(normals, GL_STATIC_DRAW);

    In order to use indexed drawing (if desired):
        obj.toggle_indexed_draw();  //index draw is off by default
        obj.set_indices(indices, GL_STATIC_DRAW);

    Specify the handle of the program object to render this Object with:
        obj.set_program(program);

    Once the program has been specified, matrix uniforms can be updated:
        obj.set_model_matrix_uniform(model_uni_name, model);
        obj.set_view_matrix_uniform(view_uni_name, view);
        obj.set_projection_matrix_uniform(projection_uni_name, projection);

    Once the program has beeen specified, textures can be enabled:
        obj.set_texture(i, tex_filename, sampler_uni_name);

    To specify which mode OpenGL should use to draw this object when glDraw* is called:
        obj.set_draw_mode(mode);  //default is GL_TRIANGLES

    To render the Object from a render loop, call this object's render() method:
        obj.render();
*/
class Object {
public:
    /* Creates a fresh Object. */
    Object();

    /* Create the VAO for the object. */
    void create_vao();

    /* Set vertices to 'vertices' (3 floats per vertex). 'usage' will be used as last arg to glBufferData(). */
    void set_vertices(const std::vector<float>& vertices, GLenum usage);
    /* Set texcoords to 'texcoords' (2 floats per texcoord). 'usage' will be used as last arg to glBufferData().*/
    void set_texcoords(const std::vector<float>& texcoords, GLenum usage);
    /* Set normals to 'normals' (3 floats per normal). 'usage' will be used as last arg to glBufferData(). */
    void set_normals(const std::vector<float>& normals, GLenum usage);

    /* Generalized version of set_vertices/texcoords/normals(). */
    void set_attribute(const std::vector<float>& data, GLenum usage, GLuint& buffer,
        unsigned int index, unsigned int size, unsigned int divisor) const;

    /* Set indices to 'indices'. 'usage' will be used as last arg to glBufferData(). */
    void set_indices(const std::vector<GLuint>& indices, GLenum usage);
    /* Toggles indexed draw on/off and returns the old state (whether indexed draw
    was previously enabled or not; returns true iff it was). */
    bool toggle_indexed_draw();

    /* Use 'program' to render this object. */
    void set_program(GLuint program);

    /* Set model matrix uniform 'model_uni_name' to 'model'. */
    void set_model_matrix_uniform(std::string model_uni_name, glm::mat4& model);
    /* Set view matrix uniform 'view_uni_name' to 'view'. */
    void set_view_matrix_uniform(std::string view_uni_name, glm::mat4& view);
    /* Set projection matrix uniform 'projection_uni_name' to 'projection'. */
    void set_projection_matrix_uniform(std::string projection_uni_name, glm::mat4& projection);

    /* Creates a new texture object that is bound to texture image unit GL_TEXTURE0 + 'i'.
    The data for this texture comes from 'tex_filename'. The sampler 'sampler_uni_name' in
    this Object's fragment shader will sample from this texture. */
    void set_texture(int i, std::string tex_filename, std::string sampler_uni_name);

    /* Set drawing mode to 'mode'. */
    void set_draw_mode(GLenum mode);

    /* Set the number of instances to render (default is 1). */
    void set_num_instances(int num_instances);

    /* Render this Object. */
    void render();

    /* Get the VAO associated with this object. */
    GLuint get_vao();
    /* Get the program associated with this object. */
    GLuint get_program();
    /* Returns model matrix for this object. */
    glm::mat4 get_model();
    /* Returns view matrix for this object. */
    glm::mat4 get_view();
    /* Returns projection matrix for this object. */
    glm::mat4 get_projection();
    /* Returns drawing mode for this object for use in glDraw* */
    GLenum get_draw_mode();
    /* Returns the number of vertices that make up this object. */
    int get_num_vertices();
    /* Returns number of indices used to draw this object. */
    int get_num_indices();
    /* Returns true iff this Object does indexed drawing. */
    bool uses_index_draw();

private:
    void set_mat4_uniform(std::string uni_name, glm::mat4& mat);
    GLuint _vao;
    GLuint _vbo_vert;  //Buffer for vertices
    GLuint _vbo_tex;   //Buffer for texcoords
    GLuint _vbo_norm;  //Buffer for normals
    GLuint _vbo_index; //Buffer for indices
    GLuint _program;
    glm::mat4 _model;
    glm::mat4 _view;
    glm::mat4 _projection;
    GLenum _draw_mode; //Enum to use when glDraw* is called for this object
    int _num_instances;
    int _num_vertices;
    int _num_indices;
    bool _use_indices; //True if doing indexed drawing with this Object
    static const int _num_tex_units = 8; //Max number of texture units to be used by Object
    GLuint _textures[_num_tex_units]; //Textures for this object
};

#endif