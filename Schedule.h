#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <vector>

#include "Rectangle.h"

struct Packet {
    std::vector<Rectangle> rectangles;
};

struct Schedule {
    std::vector<Packet> packets;
};

#endif