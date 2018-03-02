#ifndef TG_LANDSCAPEMANAGER_H
#define TG_LANDSCAPEMANAGER_H

#include "Camera.h"
#include "Configuration.h"
#include "Renderer.h"
#include "ROAMTerrain.h"
#include <omp.h>

/* Class to create and update a landscape. */
class LandscapeManager {
public:
    LandscapeManager();
    ~LandscapeManager();
    /* Creates a landscape and add it to the global renderer. */
    void create_landscape(Camera& camera);
    /* Updates landscape for the current frame. Must call create_landscape() first. */
    void update_landscape(Camera& camera);
    /* Ensures the global renderer has an updated copy of the landscape. */
    void update_in_render_list(Renderer& renderer);

private:
    ROAMTerrain* _terrain[9]; //The landscape, represented in chunks
    float _length; //Chunk length
    float _last_x, _last_z; //'lowest_extent' of the ROAMTerrain that the viewer should be on
    omp_lock_t _lock; //Stops render list from being updated while chunks are being rearranged
    int _num_allocations; //Deebug measure for memory leaks
};

#endif