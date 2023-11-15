#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <vector>

#include "Rectangle.h"

struct Packet {
    std::vector<size_t> rectangles;
};

struct Schedule {
    std::vector<Rectangle> rectangles;
    std::vector<Packet> packets;
};

#endif