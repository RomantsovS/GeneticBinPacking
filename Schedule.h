#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <vector>

#include "Rectangle.h"

struct Packet {
    Packet(size_t id) : id(id) {}
    size_t id;
    std::vector<Rectangle*> rectangles;
};

struct Schedule {
    std::vector<Rectangle> rectangles;
    std::vector<Packet> packets;
};

#endif