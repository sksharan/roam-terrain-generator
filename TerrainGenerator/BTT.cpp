#include "BTT.h"

bool BTTNodeComp::operator()(const BTTNode* a, const BTTNode* b) const {
    return a->priority < b->priority;
}