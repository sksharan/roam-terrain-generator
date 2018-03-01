#ifndef TG_GRASSPATCH_H
#define TG_GRASSPATCH_H

#include <glm/glm.hpp>
#include "Object.h"

class GrassPatch {
public:
    static void init();
    GrassPatch(glm::vec3 lowest_extent, glm::vec3 highest_extent, unsigned int step);
    Object get_object();

private:
    static Object _static_obj;
    Object _obj;
};

#endif
