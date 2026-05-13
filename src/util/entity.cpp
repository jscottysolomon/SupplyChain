#include "entity.hpp"

static int id_ = 0;

int NextId() {
    return id_++;
}