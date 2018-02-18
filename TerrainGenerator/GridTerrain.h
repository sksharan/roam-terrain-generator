#ifndef TG_GRIDTERRAIN_H
#define TG_GRIDTERRAIN_H

#include "Object.h"

/* Grid based terrain with height values determined by a noise function. */
class GridTerrain {
public:
    /* Create a GridTerrain at the origin that is 'length' x 'length' vertices in dimension.
    Vertices are 'distance' apart, and the magnitude of the height values are determined
    by 'amplitude'. */
    GridTerrain(int length, float distance, float amplitude);
    /* Get the Object associated with this GridTerrain. Only vertices, texcoords, normals,
    and indices are initialized. */
    Object get_object();

private:
    void init_object();

    int _len;       //Terrain is _length x _length vertices
    float _dist;    //Distance between vertices
    float _amp;     //Amplitude/magnitude of height values.
    Object _object; //Actual object representing this terrain
};
#endif