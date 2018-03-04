#ifndef TG_GRASSPATCH_H
#define TG_GRASSPATCH_H

#include "Configuration.h"
#include "Object.h"
#include <glm/glm.hpp>

class GrassPatch {
public:
    static void init(const Configuration& configuration);
    GrassPatch(glm::vec3 lowest_extent, glm::vec3 highest_extent, Configuration& configuration);
    Object get_object();

private:
    static Object _static_obj;
    Object _obj;
    Configuration& _configuration;
};

#endif
