#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

#include "Rectangle.h"

struct Pos {
    bool operator==(const Pos& other) const { return x == other.x && y == other.y; }

    size_t x, y;
};

inline std::ostream& operator<<(std::ostream& os, const Pos& pos) {
    return os << "pos: " << pos.x << ',' << pos.y;
}

struct RectWithPos {
    bool operator==(const RectWithPos& other) const {
        return rect == other.rect && pos == other.pos;
    }

    Rectangle* rect = nullptr;
    Pos pos;
};

inline std::ostream& operator<<(std::ostream& os, const RectWithPos& rect_with_pos) {
    return os << '[' << *rect_with_pos.rect << ',' << rect_with_pos.pos << ']';
}

struct Packet {
    Packet() = default;
    Packet(size_t i) : id(i) {}

    bool operator==(const Packet& other) const {
        return id == other.id && rectangles == other.rectangles;
    }

    size_t id;
    std::vector<RectWithPos> rectangles;
};

struct Schedule {
    bool operator==(const Schedule& other) const { return packets == other.packets; }

    bool static intersect(RectWithPos lhs, RectWithPos rhs) {
        return !(lhs.pos.y > rhs.pos.y + rhs.rect->width - 1 ||
                 lhs.pos.y + lhs.rect->width - 1 < rhs.pos.y ||
                 lhs.pos.x > rhs.pos.x + rhs.rect->height - 1 ||
                 lhs.pos.x + lhs.rect->height - 1 < rhs.pos.x);
    }

    bool hasIntersection(const RectWithPos& rect_with_pos) const {
        return std::any_of(packets.begin(), packets.end(), [&rect_with_pos](const Packet& packet) {
            return std::any_of(packet.rectangles.begin(), packet.rectangles.end(),
                               [&rect_with_pos](const RectWithPos other) {
                                   bool i = intersect(rect_with_pos, other);
                                   return i;
                               });
        });
    }

    size_t OutOfRangeSize(size_t width) const {
        return std::accumulate(
            packets.begin(), packets.end(), 0u, [width](size_t sum, const Packet& packet) {
                return sum + std::accumulate(
                                 packet.rectangles.begin(), packet.rectangles.end(), 0u,
                                 [width](size_t sum2, const RectWithPos& rect_with_pos) {
                                     size_t rect_end =
                                         rect_with_pos.pos.y + rect_with_pos.rect->width;
                                     return sum2 + rect_end > width
                                                ? (rect_end - width) * rect_with_pos.rect->height
                                                : 0u;
                                 });
            });
    }

    std::vector<Packet> packets;
};

#endif