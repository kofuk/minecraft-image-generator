#ifndef GENERATOR_HH
#define GENERATOR_HH

#include "worker.hh"

namespace generator {
    /* generates 256x256 image */
    void generate_256 (QueuedItem *item);
} // namespace generator

#endif