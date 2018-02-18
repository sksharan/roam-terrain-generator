#ifndef TG_OBJECTTEST_H
#define TG_OBJECTTEST_H

/* Class with test functions that create objects to render. */
class ObjectTest {
public:
    /* Create triangle with or without indexed drawing. */
    static bool gen_triangle(bool use_indices);
    /* Create triangle that uses shaders with matrix uniforms. */
    static bool use_matrices();
    /* Create quad with GL_TRIANGLES or GL_TRIANGLE_FAN drawing mode. */
    static bool use_indices_and_matrices(bool use_triangle_fan);
    /* Create GridTerrain object. */
    static bool grid_terrain();
    /* Create ROAMTerrain object. */
    static bool roam_terrain();
};

#endif