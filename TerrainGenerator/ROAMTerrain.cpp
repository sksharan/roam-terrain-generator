#include "ROAMTerrain.h"
#include "Globals.h"
#include "Utils.h"
#include <algorithm>
#include <ctime>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

/* A template object from which all other ROAMTerrain objects are made. This allows for
rapid creation of multiple ROAMTerrain objects. */
Object ROAMTerrain::_static_obj;

void ROAMTerrain::init() {
    /* Initialize the static 'template' object. */
    GLuint shaders[3];
    shaders[0] = Utils::create_shader("shaders/roamterrain.vert", GL_VERTEX_SHADER);
    shaders[1] = Utils::create_shader("shaders/roamterrain.geo", GL_GEOMETRY_SHADER);
    shaders[2] = Utils::create_shader("shaders/roamterrain.frag", GL_FRAGMENT_SHADER);
	assert(shaders[0] > 0 && shaders[1] > 0 && shaders[2] > 0);
    GLuint program = Utils::create_program(shaders, 3);
    assert(program > 0);
    _static_obj.create_vao();
    _static_obj.set_program(program);
    glm::mat4 model = glm::mat4();
    glm::mat4 view = glm::mat4();
    glm::mat4 projection = glm::perspective(45.0, 1.0, 0.1, 5000.0);
    _static_obj.set_model_matrix_uniform("model", model);
    _static_obj.set_view_matrix_uniform("view", view);
    _static_obj.set_projection_matrix_uniform("projection", projection);
    _static_obj.set_texture(0, "textures/grass.jpg", "tex0");
    _static_obj.set_texture(1, "textures/dirt.jpg", "tex1");
    _static_obj.set_texture(2, "textures/rock.jpg", "tex2");
    _static_obj.set_texture(3, "textures/grass_blades.png", "tex3");
}

ROAMTerrain::ROAMTerrain(glm::vec3 lowest_extent, glm::vec3 highest_extent) {
    _tree = NULL;
    _obj = _static_obj;          //make a copy of the static 'template' object
    _lowest_extent = lowest_extent;
    _highest_extent = highest_extent;
    _width = _highest_extent.x - _lowest_extent.x;
    _length = _lowest_extent.z - _highest_extent.z;
    _var_threshold = 0.0005f;    //determines terrain "accuracy"
    _max_split_iter = 100;       //max number of times to loop in begin_split()
    _max_triangles = 7500;       //max number of triangles to have in terrain
    _num_allocations = 0;        //help guard against memory leaks; should be 0 afer delete_tree() is called
    _last_camera_pos = Globals::camera.get_pos();  //viewer position when '_tree' was last constructed
    _tree_regen_dist = 300.0f;   //how far we can move before _tree needs to be recreated
    _first_frame = true;         //true only the first frame, when '_tree' is first constructed
    _tex_repeat_factor = 8.0f;   //determines how often texture should repeat on landscape
    _obj_changed = true;
    /* Create the initial terrain. */
    calc();
}

Object ROAMTerrain::get_object() {
    /* Initialize _obj for first time if needed. */
    if (_first_frame) {
        _obj.create_vao(); //make a new vao for this object
        _first_frame = false;
    }
    /* Set vertices, texcoords, and normals if needed. */
    if (_obj_changed) {
        _obj.set_vertices(_verts, GL_STREAM_DRAW);
        _obj.set_texcoords(_texcoords, GL_STREAM_DRAW);
        _obj.set_normals(_normals, GL_STREAM_DRAW);
        _obj_changed = false;
    }
    return _obj;
}

void ROAMTerrain::calc() {
    /* If the viewer has not moved too far since the last BTT was constructed, we continue
    splitting the same BTT. Otherwise, a new BTT has to be constructed. */
    if (!_first_frame && glm::distance(_last_camera_pos, Globals::camera.get_pos()) < _tree_regen_dist) {
        begin_split();
        return;
    }

    /* Update viewer position. */
    _last_camera_pos = Globals::camera.get_pos();

    /* Destroy the old bintree.  */
    delete_tree(_tree);
    assert(_num_allocations == 0);

    /* Clear old split set. */
    _split_set.clear();

    /* Create new tree. */
    _tree = alloc_BTTNode();
    /* Initialize the parent node of the BTT. The parent node is special in that it doesn't represent
    a triangle itself. It merely connects the two triangles that form the base terrain. */
    _tree->val = NULL;
    _tree->priority = -1;
    _tree->leftneighbor = NULL;
    _tree->rightneighbor = NULL;
    _tree->baseneighbor = NULL;
    _tree->leftchild = alloc_BTTNode();
    _tree->rightchild = alloc_BTTNode();
    /* Initialize the left child of the parent. The apex of this triangle is '_lowest_extent'. */
    _tree->leftchild->val = alloc_BTTVal();
    _tree->leftchild->val->a = _lowest_extent;
    _tree->leftchild->val->a.y = Utils::get_value(_tree->leftchild->val->a.x, 0, _tree->leftchild->val->a.z);
    _tree->leftchild->val->v0 = _lowest_extent + glm::vec3(_width, 0, 0);
    _tree->leftchild->val->v0.y = Utils::get_value(_tree->leftchild->val->v0.x, 0, _tree->leftchild->val->v0.z);
    _tree->leftchild->val->v1 = _lowest_extent - glm::vec3(0, 0, _length);
    _tree->leftchild->val->v1.y = Utils::get_value(_tree->leftchild->val->v1.x, 0, _tree->leftchild->val->v1.z);
    _tree->leftchild->priority = get_priority(_tree->leftchild);
    _tree->leftchild->leftneighbor = NULL;
    _tree->leftchild->rightneighbor = NULL;
    _tree->leftchild->baseneighbor = _tree->rightchild;
    _tree->leftchild->leftchild = NULL;
    _tree->leftchild->rightchild = NULL;
    /* Initialize the right child of the parent. The apex of this triangle is '_highest_extent'. */
    _tree->rightchild->val = alloc_BTTVal();
    _tree->rightchild->val->a = _highest_extent;
    _tree->rightchild->val->a.y = Utils::get_value(_tree->rightchild->val->a.x, 0, _tree->rightchild->val->a.z);
    _tree->rightchild->val->v0 = _highest_extent - glm::vec3(_width, 0, 0);
    _tree->rightchild->val->v0.y = Utils::get_value(_tree->rightchild->val->v0.x, 0, _tree->rightchild->val->v0.z);
    _tree->rightchild->val->v1 = _highest_extent + glm::vec3(0, 0, _length);
    _tree->rightchild->val->v1.y = Utils::get_value(_tree->rightchild->val->v1.x, 0, _tree->rightchild->val->v1.z);
    _tree->rightchild->priority = get_priority(_tree->rightchild);
    _tree->rightchild->leftneighbor = NULL;
    _tree->rightchild->rightneighbor = NULL;
    _tree->rightchild->baseneighbor = _tree->leftchild;
    _tree->rightchild->leftchild = NULL;
    _tree->rightchild->rightchild = NULL;

    /* Add base triangulation to the split set. */
    _split_set.insert(_tree->leftchild);
    _split_set.insert(_tree->rightchild);

    /* Beginning splitting. For the first frame, we immediately initialize the object. */
    begin_split();
    if (_first_frame) {
        init_object_data();
    }
}

ROAMTerrain::~ROAMTerrain() {
    /* Delete all memory allocated for '_tree'. */
    delete_tree(_tree);
    assert(_num_allocations == 0);
}

/* Allocates memory for a BTTNode and returns a pointer to it. */
BTTNode* ROAMTerrain::alloc_BTTNode() {
    BTTNode* ptr = (BTTNode*)malloc(sizeof(BTTNode));
    if (ptr == NULL) {
        std::cerr << "Failed to allocate memory for BTTNode" << std::endl;
    }
    _num_allocations++;
    return ptr;
}

/* Allocates memory for a BTTVal and returns a pointer to it. */
BTTVal* ROAMTerrain::alloc_BTTVal() {
    BTTVal* ptr = (BTTVal*)malloc(sizeof(BTTVal));
    if (ptr == NULL) {
        std::cerr << "Failed to allocate memory for BTTVal" << std::endl;
    }
    _num_allocations++;
    return ptr;
}

/* Begins the splitting algorithm. */
void ROAMTerrain::begin_split() {
    int old_size = _split_set.size(); //prior size of the split set

    /* Repeadly split triangles from our split queue until our terrain is "accurate enough", until
    our split set is empty, or until we have reached the maximum number of iterations or triangles allowed. */
    for (int i = 0; i < _max_split_iter && !_split_set.empty() && _split_set.size() < _max_triangles / 3; i++) {
        BTTNode* node = *_split_set.rbegin(); //get highest priority item from split set
        if (node->priority < _var_threshold) {
            break;  //terrain is "accurate enough"
        }
        split(node);
    }

    /* Update object if we performed a split and there are enough vertices. */
    if (_split_set.size() != old_size && _split_set.size() >= _max_triangles / 3) {
        init_object_data();
    }
}

/* Free the memory in the BTT starting at 'node.' */
void ROAMTerrain::delete_tree(BTTNode* node) {
    if (node == NULL) {
        return;
    }
    delete_tree(node->leftchild);
    delete_tree(node->rightchild);

    /* Free the memory used by the BTTNode. '_tree' does not have a value. */
    if (node != _tree) {
        free(node->val); _num_allocations--;
        node->val = NULL;
    }
    free(node); _num_allocations--;
    node = NULL;
}

/* Initialize data for _obj. */
void ROAMTerrain::init_object_data() {
    /* Clear old vertices, texcoords, and normals. */
    _verts.clear();
    _texcoords.clear();
    _normals.clear();

    /* Create the new vertices, texcoords, normals. The split set always contains the
    root triangles of the BTT, so we simply iterate over it to get the triangles. */
    for (std::multiset<BTTNode*, BTTNodeComp>::iterator iter = _split_set.begin(); iter != _split_set.end(); ++iter) {
        BTTNode* node = *iter;
        /* Add vertices. */
        _verts.push_back(node->val->a.x);
        _verts.push_back(node->val->a.y);
        _verts.push_back(node->val->a.z);
        _verts.push_back(node->val->v0.x);
        _verts.push_back(node->val->v0.y);
        _verts.push_back(node->val->v0.z);
        _verts.push_back(node->val->v1.x);
        _verts.push_back(node->val->v1.y);
        _verts.push_back(node->val->v1.z);
        /* Add texcoords. */
        _texcoords.push_back(node->val->a.x / _length * _tex_repeat_factor);
        _texcoords.push_back(node->val->a.z / _length * _tex_repeat_factor);
        _texcoords.push_back(node->val->v0.x / _length * _tex_repeat_factor);
        _texcoords.push_back(node->val->v0.z / _length * _tex_repeat_factor);
        _texcoords.push_back(node->val->v1.x / _length * _tex_repeat_factor);
        _texcoords.push_back(node->val->v1.z / _length * _tex_repeat_factor);
        /* Add normals. */
        glm::vec3 a_to_v0 = glm::normalize(node->val->v0 - node->val->a);
        glm::vec3 a_to_v1 = glm::normalize(node->val->v1 - node->val->a);
        glm::vec3 normal = glm::cross(a_to_v0, a_to_v1);
        if (normal.y < 0.0f) {  //ensure the normal always faces upwards
            normal = -normal;
        }
        normal = glm::normalize(normal);
        for (int i = 0; i < 3; i++) {
            _normals.push_back(normal.x);
            _normals.push_back(normal.y);
            _normals.push_back(normal.z);
        }
    }

    /* Indicate that the data for _obj has changed. */
    _obj_changed = true;
}

/* Returns iterator to 'node' in the split set, or _split_set.end() if it cannot be found. */
std::multiset<BTTNode*>::iterator ROAMTerrain::find_node(BTTNode* node) {
    for (std::multiset<BTTNode*, BTTNodeComp>::iterator iter = _split_set.lower_bound(node); iter != _split_set.end(); ++iter) {
        if (*iter == node) {
            return iter;
        }
    }
    return _split_set.end();
}

/* Sets up triangles for splitting. Adapted from psuedocode at
https://www.longbowgames.com/seumas/progbintri.html */
void ROAMTerrain::split(BTTNode* node) {
    if (node->baseneighbor != NULL) {
        /* If 'node' and its neighbor do not form a diamond, we need to force splits until they do. */
        if (node->baseneighbor->baseneighbor != node) {
            split(node->baseneighbor);
        }
        /* Split across the diamond and finish setting up relationships between the new triangles. */
        split2(node);
        split2(node->baseneighbor);
        node->leftchild->rightneighbor = node->baseneighbor->rightchild;
        node->rightchild->leftneighbor = node->baseneighbor->leftchild;
        node->baseneighbor->leftchild->rightneighbor = node->rightchild;
        node->baseneighbor->rightchild->leftneighbor = node->leftchild;
        /* Add the newly created triangles to the split set. */
        _split_set.insert(node->leftchild);
        _split_set.insert(node->rightchild);
        _split_set.insert(node->baseneighbor->leftchild);
        _split_set.insert(node->baseneighbor->rightchild);
        /* Remove 'node' and its base neighbor from the split set. */
        _split_set.erase(find_node(node));
        _split_set.erase(find_node(node->baseneighbor));
        return;
    }
    /* If 'node' has no base neighbor, then we just split 'node'. */
    split2(node);
    node->leftchild->rightneighbor = NULL;
    node->rightchild->leftneighbor = NULL;
    /* Add the newly created triangles to the split set. */
    _split_set.insert(node->leftchild);
    _split_set.insert(node->rightchild);
    /* Remove 'node' from the split set. */
    _split_set.erase(find_node(node));
}

/* Performs the actual splitting of the triangle represented by 'node'. Adapted from psuedocode
at https://www.longbowgames.com/seumas/progbintri.html */
void ROAMTerrain::split2(BTTNode* node) {
    /* Get the midpoint between tri->v0 and tri->v1; this is the location of the new vertex
    introduced by the split. */
    BTTVal* tri = node->val;
    glm::vec3 midpoint((tri->v0.x + tri->v1.x) / 2.0f,
        (tri->v0.y + tri->v1.y) / 2.0f,
        (tri->v0.z + tri->v1.z) / 2.0f);

    /* Allocate memory for the new child triangles. */
    node->leftchild = alloc_BTTNode();
    node->rightchild = alloc_BTTNode();
    /* Initialize the new left child of 'node'. */
    node->leftchild->val = alloc_BTTVal();
    node->leftchild->val->a = midpoint;
    node->leftchild->val->a.y = Utils::get_value(node->leftchild->val->a.x, 0, node->leftchild->val->a.z);
    node->leftchild->val->v0 = node->val->a;
    node->leftchild->val->v0.y = Utils::get_value(node->leftchild->val->v0.x, 0, node->leftchild->val->v0.z);
    node->leftchild->val->v1 = node->val->v0;
    node->leftchild->val->v1.y = Utils::get_value(node->leftchild->val->v1.x, 0, node->leftchild->val->v1.z);
    node->leftchild->priority = get_priority(node->leftchild);
    node->leftchild->leftneighbor = node->rightchild;
    node->leftchild->baseneighbor = node->leftneighbor;
    node->leftchild->leftchild = NULL;
    node->leftchild->rightchild = NULL;
    /* Initialize the new right child of 'node'. */
    node->rightchild->val = alloc_BTTVal();
    node->rightchild->val->a = midpoint;
    node->rightchild->val->a.y = Utils::get_value(node->rightchild->val->a.x, 0, node->rightchild->val->a.z);
    node->rightchild->val->v0 = node->val->v1;
    node->rightchild->val->v0.y = Utils::get_value(node->rightchild->val->v0.x, 0, node->rightchild->val->v0.z);
    node->rightchild->val->v1 = node->val->a;
    node->rightchild->val->v1.y = Utils::get_value(node->rightchild->val->v1.x, 0, node->rightchild->val->v1.z);
    node->rightchild->priority = get_priority(node->rightchild);
    node->rightchild->rightneighbor = node->leftchild;
    node->rightchild->baseneighbor = node->rightneighbor;
    node->rightchild->leftchild = NULL;
    node->rightchild->rightchild = NULL;

    /* Modify neighbor relationships to accomodate for the newly introduced triangles. */
    if (node->leftneighbor != NULL) {
        if (node->leftneighbor->baseneighbor == node) {
            node->leftneighbor->baseneighbor = node->leftchild;
        }
        else if (node->leftneighbor->leftneighbor == node) {
            node->leftneighbor->leftneighbor = node->leftchild;
        }
        else {
            node->leftneighbor->rightneighbor = node->leftchild;
        }
    }
    if (node->rightneighbor != NULL) {
        if (node->rightneighbor->baseneighbor == node) {
            node->rightneighbor->baseneighbor = node->rightchild;
        }
        else if (node->rightneighbor->leftneighbor == node) {
            node->rightneighbor->leftneighbor = node->rightchild;
        }
        else {
            node->rightneighbor->rightneighbor = node->rightchild;
        }
    }
}

/* Returns the priority of 'node'.*/
float ROAMTerrain::get_priority(BTTNode* node) {
    /* If triangle->a is the apex of the triangle, then the hypotenuse
    is given by the line segment that connects triangle->v0 and
    triangle->v1. We calculate the current height at the midpoint of the
    hypotenuse and compare it to the real reference height (as returned by
    Utils::get_value()). The difference between these two values is our variance.
    We divide the variance by the viewer's distance from the triangle to get
    the final priority. */

    BTTVal* triangle = node->val;

    /* Calculate variance. */
    float curr_height = (triangle->v0.y + triangle->v1.y) / 2.0f;
    float real_height = Utils::get_value((triangle->v0.x + triangle->v1.x) / 2.0f, 0,
        (triangle->v0.z + triangle->v1.z) / 2.0f);
    float variance = fabs(real_height - curr_height);

    /* Calculate viewer's distance from the triangle. */
    float distance = glm::distance(glm::vec3(triangle->a.x, triangle->a.y, triangle->a.z),
        Globals::camera.get_pos());

    /* Return the final priority. */
    if (distance > -0.002f && distance < 0.002) {  //case when distance is too close to 0
        return variance / 0.002f;
    }
    return variance / distance;
}

