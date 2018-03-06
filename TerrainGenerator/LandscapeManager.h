#ifndef TG_LANDSCAPEMANAGER_H
#define TG_LANDSCAPEMANAGER_H

#include "Camera.h"
#include "Configuration.h"
#include "Renderer.h"
#include "ROAMTerrain.h"
#include <mutex>

/* Class to create and update a landscape. */
class LandscapeManager {
public:
    LandscapeManager(Camera& camera, Configuration& configuration, Renderer& renderer);
    ~LandscapeManager();
    /* Creates a landscape and add it to the global renderer. */
    void create_landscape();
    /* Updates landscape for the current frame. Must call create_landscape() first. */
    void update_landscape();
    /* Ensures the global renderer has an updated copy of the landscape. */
    void update_in_render_list();

private:
    Camera& _camera;
    Configuration& _configuration;
    Renderer& _renderer;
    ROAMTerrain* _terrain[9]; //The landscape, represented in chunks
    float _length; //Chunk length
    float _last_x, _last_z; //'lowest_extent' of the ROAMTerrain that the viewer should be on
    int _num_allocations; //Deebug measure for memory leaks
    std::mutex _mutex;

    ROAMTerrain* gen_ROAMTerrain(glm::vec3 lowest_extent, glm::vec3 highest_extent);
};

#endif