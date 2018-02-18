#ifndef TG_ROAMTERRAIN_H
#define TG_ROAMTERRAIN_H

#include "BTT.h"
#include "Object.h"
#include <set>

/* Terrain that uses a split-only version of the ROAM algorithm for level of detail. 
The terrain is represented internally as a binary triangle tree (BTT), or "bintree."
A multiset is used in place of the priority queue since any element in a multiset can
be deleted efficiently (logarithmic in the size of the set). The highest priority
node in a multiset can also be found in constant time by calling rbegin() if the correct
comparison function is provided (this implementation uses the BTTNodeComp class in 
BTT.h to do so). */
class ROAMTerrain {
public:
    /* Prepares the ROAMTerrain class for use. Must call before any ROAMTerrains are created. */
    static void init();
    /* Creates terrain that spans from 'lowest_extent' to 'highest_extent'. */
    ROAMTerrain(glm::vec3 lowest_extent, glm::vec3 highest_extent);
    /* Returns a fully created Object associated with this ROAMTerrain. Must be called by
    the main thread associated with the OpenGL context.*/
    Object get_object();
    /* Recalculate the data for this terrain for the current frame. */
    void calc();
    /* Free all memory used by this terrain. */
    ~ROAMTerrain();
private:
    BTTNode* alloc_BTTNode();
    BTTVal* alloc_BTTVal();
    void begin_split();
    void delete_tree(BTTNode* node);
    void init_object_data();
    std::multiset<BTTNode*>::iterator find_node(BTTNode* node);
    void split(BTTNode* node);
    void split2(BTTNode* node);
    static float get_priority(BTTNode* node);

    static Object _static_obj; //Template object
    BTTNode* _tree; //Root of the BTT which does not represent any triangle
    Object _obj;
    std::vector<float> _verts;
    std::vector<float> _texcoords;
    std::vector<float> _normals;
    std::multiset<BTTNode*, BTTNodeComp> _split_set; //Set used in place of the priority queue
    glm::vec3 _lowest_extent;
    glm::vec3 _highest_extent;
    float _width;
    float _length;
    float _var_threshold; //Determines "accuracy" of the mesh
    int _max_split_iter; //Maximum number of iterations allowed in begin_split()
    int _max_triangles; //Loose upper bound on number of triangles allowed
    int _num_allocations; //Should be 0 after delete_tree() is called
    glm::vec3 _last_camera_pos; //Position of the camera the last time the BTT was constructed
    float _tree_regen_dist; //How far we can move before _tree needs to be recreated
    bool _first_frame; //True only when this terrain is first created
    float _tex_repeat_factor; //Modifies how often a texture repeats across the terrain
    bool _obj_changed; //True iff the object has been modified since the last time get_object() was called
};

#endif