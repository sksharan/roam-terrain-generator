#include "GridTerrain.h"
#include "Utils.h"
#include <vector>

GridTerrain::GridTerrain(int length, float distance, float amplitude) {
    _len = length;
    _dist = distance;
    _amp = amplitude;
    init_object();
}

Object GridTerrain::get_object() {
    return _object;
}

/* Initialize '_object'. */
void GridTerrain::init_object() {
    std::vector<float> vertices, texcoords, normals;
    glm::vec3 vertex;
    for (int i = 0; i < _len; i++) {
        vertex.x = i * _dist;

        for (int j = 0; j < _len; j++) {
            vertex.z = -(j * _dist);
            vertex.y = Utils::get_value(vertex.x, 0, vertex.z) * _amp;

            /* Add vertex. */
            vertices.push_back(vertex.x);
            vertices.push_back(vertex.y);
            vertices.push_back(vertex.z);
            /* Add texcoord. */
            texcoords.push_back(0);
            texcoords.push_back(0);
            /* Add normal. */
            normals.push_back(0);
            normals.push_back(1);
            normals.push_back(0);
        }
    }
    _object.set_vertices(vertices, GL_STATIC_DRAW);
    _object.set_texcoords(texcoords, GL_STATIC_DRAW);
    _object.set_normals(normals, GL_STATIC_DRAW);

    /* Set indices. */
    std::vector<GLuint> indices;
    for (int i = 0; i < _len - 1; i++) {
        /* For degenerate triangle. */
        if (i > 0) {
            indices.push_back(i * _len);
        }
        for (int j = 0; j < _len; j++) {
            indices.push_back(i * _len + j);
            indices.push_back((i + 1) * _len + j);
        }
        /* For degenerate triangle. */
        if (i < _len - 1) {
            indices.push_back((i + 1) * _len + (_len - 1));
        }
    }
    _object.set_indices(indices, GL_STATIC_DRAW);
    _object.toggle_indexed_draw();
    _object.set_draw_mode(GL_TRIANGLE_STRIP);
}
