#include "LandscapeManager.h"
#include "Globals.h"
#include <iostream>

/* The landscape is composed of nine chunks, call these chunks 0-8. The
viewer is always in the center chunk, say chunk 0. The landscape would
therefore have the following layout:
   ^
   |  8  1  2
-z |  7  0  3
   |  6  5  4
    ------->
        +x
If the viewer moves into another chunk, say chunk 3, then the chunks are
updated so that chunk 3 becomes the new center chunk 0. In this case,
the old chunks 6-8 are no longer needed and can be destroyed. */

/** Rounds n down to nearest multiple of m. See https://gist.github.com/aslakhellesoy/1134482 */
int round_down(int n, int m) {
    return n >= 0 ? (n / m) * m : ((n - m + 1) / m) * m;
}

/** Rounds n up to nearest multiple of m. See https://gist.github.com/aslakhellesoy/1134482 */
int round_up(int n, int m) {
    return n >= 0 ? ((n + m - 1) / m) * m : (n / m) * m;
}

LandscapeManager::LandscapeManager() {
    for (int i = 0; i < 9; i++) {
        _terrain[i] = NULL;
    }
    _length = 1500.0f;
    _last_x = 0; //center chunk has lowest_extent.x = 0
    _last_z = 0; //center chunk has lowest_extent.z = 0
    _num_allocations = 0;
    omp_init_lock(&_lock);
}

LandscapeManager::~LandscapeManager() {
    for (int i = 0; i < 9; i++) {
        if (_terrain[i] != NULL) {
            delete _terrain[i];
            _num_allocations--;
        }
    }
    assert(_num_allocations == 0);
}

void LandscapeManager::create_landscape() {
    /* Find lowest extent of the chunk the viewer is on. */
    int x = round_down(Globals::camera.get_pos().x, _length);
    int z = round_up(Globals::camera.get_pos().z, _length);
    /* Create 9 _length x _length chunks arraged as mentioned in the comment above. */
    _terrain[0] = new ROAMTerrain(glm::vec3(x, 0, z), glm::vec3(x + _length, 0, z - _length));
    _terrain[1] = new ROAMTerrain(glm::vec3(x, 0, z - _length), glm::vec3(x + _length, 0, z - (2 * _length)));
    _terrain[2] = new ROAMTerrain(glm::vec3(x + _length, 0, z - _length), glm::vec3(x + (2 * _length), 0, z - (2 * _length)));
    _terrain[3] = new ROAMTerrain(glm::vec3(x + _length, 0, z), glm::vec3(x + (2 * _length), 0, z - _length));
    _terrain[4] = new ROAMTerrain(glm::vec3(x + _length, 0, z + _length), glm::vec3(x + (2 * _length), 0, z));
    _terrain[5] = new ROAMTerrain(glm::vec3(x, 0, z + _length), glm::vec3(x + _length, 0, z));
    _terrain[6] = new ROAMTerrain(glm::vec3(x - _length, 0, z + _length), glm::vec3(x, 0, z));
    _terrain[7] = new ROAMTerrain(glm::vec3(x - _length, 0, z), glm::vec3(x, 0, z - _length));
    _terrain[8] = new ROAMTerrain(glm::vec3(x - _length, 0, z - _length), glm::vec3(x, 0, z - (2 * _length)));
    _num_allocations += 9;
}

void LandscapeManager::update_landscape() {
    if (_terrain[0] == NULL) { //if create_landscape() not yet called
        return;
    }
    /* Temporarily holds the updated terrain chunks if needed. */
    ROAMTerrain* temp[9] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    /* Find lowest extent of the chunk the viewer is on. */
    int x = round_down(Globals::camera.get_pos().x, _length);
    int z = round_up(Globals::camera.get_pos().z, _length);
    /* Booleans to indicate which chunk the viewer moved into. */
    bool moved_west = false;
    bool moved_north = false;
    bool moved_east = false;
    bool moved_south = false;

    /* Moved into western chunk */
    if (x < _last_x) {
        std::cout << "moving into western chunk" << std::endl;
        moved_west = true;
        temp[0] = _terrain[7];
        temp[1] = _terrain[8];
        temp[2] = _terrain[1];
        temp[3] = _terrain[0];
        temp[4] = _terrain[5];
        temp[5] = _terrain[6];
        temp[6] = new ROAMTerrain(glm::vec3(x - _length, 0, z + _length), glm::vec3(x, 0, z));
        temp[7] = new ROAMTerrain(glm::vec3(x - _length, 0, z), glm::vec3(x, 0, z - _length));
        temp[8] = new ROAMTerrain(glm::vec3(x - _length, 0, z - _length), glm::vec3(x, 0, z - (2 * _length)));
        temp[6]->calc();
        temp[7]->calc();
        temp[8]->calc();
    }
    /* Moved into northern chunk */
    else if (z < _last_z) {
        std::cout << "moving into northern chunk" << std::endl;
        moved_north = true;
        temp[0] = _terrain[1];
        temp[1] = new ROAMTerrain(glm::vec3(x, 0, z - _length), glm::vec3(x + _length, 0, z - (2 * _length)));
        temp[2] = new ROAMTerrain(glm::vec3(x + _length, 0, z - _length), glm::vec3(x + (2 * _length), 0, z - (2 * _length)));
        temp[3] = _terrain[2];
        temp[4] = _terrain[3];
        temp[5] = _terrain[0];
        temp[6] = _terrain[7];
        temp[7] = _terrain[8];
        temp[8] = new ROAMTerrain(glm::vec3(x - _length, 0, z - _length), glm::vec3(x, 0, z - (2 * _length)));
    }
    /* Moved into eastern chunk */
    else if (x > _last_x) {
        std::cout << "moving into eastern chunk" << std::endl;
        moved_east = true;
        temp[0] = _terrain[3];
        temp[1] = _terrain[2];
        temp[2] = new ROAMTerrain(glm::vec3(x + _length, 0, z - _length), glm::vec3(x + (2 * _length), 0, z - (2 * _length)));
        temp[3] = new ROAMTerrain(glm::vec3(x + _length, 0, z), glm::vec3(x + (2 * _length), 0, z - _length));
        temp[4] = new ROAMTerrain(glm::vec3(x + _length, 0, z + _length), glm::vec3(x + (2 * _length), 0, z));
        temp[5] = _terrain[4];
        temp[6] = _terrain[5];
        temp[7] = _terrain[0];
        temp[8] = _terrain[1];
    }
    /* Moved into southern chunk */
    else if (z > _last_z) {
        std::cout << "moving into southern chunk" << std::endl;
        moved_south = true;
        temp[0] = _terrain[5];
        temp[1] = _terrain[0];
        temp[2] = _terrain[3];
        temp[3] = _terrain[4];
        temp[4] = new ROAMTerrain(glm::vec3(x + _length, 0, z + _length), glm::vec3(x + (2 * _length), 0, z));
        temp[5] = new ROAMTerrain(glm::vec3(x, 0, z + _length), glm::vec3(x + _length, 0, z));
        temp[6] = new ROAMTerrain(glm::vec3(x - _length, 0, z + _length), glm::vec3(x, 0, z));
        temp[7] = _terrain[6];
        temp[8] = _terrain[7];
    }

    /* Update lowest extent values. */
    _last_x = x;
    _last_z = z;
    /* Update the terrain chunks if needed. Delete chunks no longer needed. */
    omp_set_lock(&_lock);
    if (temp[0] != NULL) {
        if (moved_west) {
            delete _terrain[2];
            delete _terrain[3];
            delete _terrain[4];
        }
        else if (moved_north) {
            delete _terrain[4];
            delete _terrain[5];
            delete _terrain[6];
        }
        else if (moved_east) {
            delete _terrain[6];
            delete _terrain[7];
            delete _terrain[8];
        }
        else if (moved_south) {
            delete _terrain[1];
            delete _terrain[2];
            delete _terrain[8];
        }
        for (int i = 0; i < 9; i++) {
            _terrain[i] = temp[i];
        }
    }
    omp_unset_lock(&_lock);
    /* Perform the terrain update for the current frame. */
    for (int i = 0; i < 9; i++) {
        _terrain[i]->calc();
    }
}

void LandscapeManager::update_in_render_list() {
    omp_set_lock(&_lock);
    Globals::renderer.clear();
    for (int i = 0; i < 9; i++) {
        Globals::renderer.add_object(_terrain[i]->get_object());
    }
    omp_unset_lock(&_lock);
}
