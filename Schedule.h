#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <algorithm>
#include <memory>
#include <vector>

#include "Rectangle.h"

struct Packet {
    Packet(size_t id) : id(id) {}
    size_t id;
    std::vector<Rectangle*> rectangles;
};

struct Schedule {
    bool hasIntersection(const Rectangle& rect) const {
        return std::any_of(packets.begin(), packets.end(), [&rect](const Packet& packet) {
            return std::any_of(packet.rectangles.begin(), packet.rectangles.end(),
                               [&rect](const Rectangle* other) { return rect.intersect(*other); });
        });
    }

    std::vector<std::unique_ptr<Rectangle>> rectangles;
    std::vector<Packet> packets;
};

#endif