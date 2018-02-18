#ifndef TG_BTT_H
#define TG_BTT_H

#include <glm/glm.hpp>

/* Represents a triangle. Used as the value in a BTTNode. */
class BTTVal {
public:
    glm::vec3 a;  //Apex vertex
    glm::vec3 v0; //Left vertex
    glm::vec3 v1; //Right vertex
};

/* Node for a binary triangle tree. */
class BTTNode {
public:
    BTTVal* val;
    float priority;
    BTTNode* leftneighbor;
    BTTNode* rightneighbor;
    BTTNode* baseneighbor;
    BTTNode* leftchild;
    BTTNode* rightchild;
};

/* Comparator for BTTNodes. */
class BTTNodeComp {
public:
    /* The smaller BTTNode is the one with the smaller priority. */
    bool operator()(const BTTNode* a, const BTTNode* b) const;
};

#endif