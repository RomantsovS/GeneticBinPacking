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
    return os << '[' << *rect_with_pos.rect << ", " << rect_with_pos.pos << ']';
}

struct Packet {
    Packet() = default;
    Packet(size_t i) : id(i) {}

    bool operator==(const Packet& other) const {
        return id == other.id && rectangles == other.rectangles;
    }

    size_t width() const {
        return std::accumulate(rectangles.begin(), rectangles.end(), 0u,
                               [](size_t sum, const RectWithPos& rect_with_pos) {
                                   return sum + rect_with_pos.rect->width;
                               });
    }

    size_t id = -1;
    std::vector<RectWithPos> rectangles;
    size_t max_width = 0;
};

class Schedule {
   public:
    Schedule(size_t num_packets) {
        packets.reserve(num_packets);
        for (size_t i = 0; i < num_packets; ++i) {
            packets.emplace_back(i);
        }
    }
    bool operator==(const Schedule& other) const { return packets == other.packets; }

    void addPacket(const Packet& packet) { packets.push_back(packet); }
    void addRectangle(size_t packet_id, const RectWithPos& rect_with_pos) {
        auto& packet = packets.at(packet_id);
        packet.rectangles.push_back(rect_with_pos);
        for (size_t i = 0; i < rect_with_pos.rect->height; ++i) {
            packets.at(packet_id + i).max_width = rect_with_pos.pos.y + rect_with_pos.rect->width;
        }
    }

    const std::vector<Packet>& getPackets() const { return packets; }

    bool static intersect(RectWithPos lhs, RectWithPos rhs) {
        return !(lhs.pos.y > rhs.pos.y + rhs.rect->width - 1 ||
                 lhs.pos.y + lhs.rect->width - 1 < rhs.pos.y ||
                 lhs.pos.x > rhs.pos.x + rhs.rect->height - 1 ||
                 lhs.pos.x + lhs.rect->height - 1 < rhs.pos.x);
    }

    bool hasIntersection(const RectWithPos& rect_with_pos) const {
        return std::any_of(packets.begin(), packets.end(), [&rect_with_pos](const Packet& packet) {
            return std::any_of(packet.rectangles.begin(), packet.rectangles.end(),
                               [&rect_with_pos](const RectWithPos& other) {
                                   bool i = intersect(rect_with_pos, other);
                                   return i;
                               });
        });
    }

    size_t OutOfRangeSize(size_t width) const {
        return std::accumulate(
            packets.begin(), packets.end(), 0u, [width](size_t sum, const Packet& packet) {
                return sum +
                       std::accumulate(
                           packet.rectangles.begin(), packet.rectangles.end(), 0u,
                           [width](size_t sum2, const RectWithPos& rect_with_pos) {
                               size_t rect_end = rect_with_pos.pos.y + rect_with_pos.rect->width;
                               return sum2 + (rect_end > width
                                                  ? (rect_end - width) * rect_with_pos.rect->height
                                                  : 0u);
                           });
            });
    }

    void print() {
        size_t total_area = 0;

        for (const auto& packet : packets) {
            std::cout << "Packet #" << packet.id << " width: " << packet.width() << '\n';
            for (const auto& rect_with_pos : packet.rectangles) {
                total_area += rect_with_pos.rect->height * rect_with_pos.rect->width;
                std::cout << "\t" << rect_with_pos << '\n';
            }
        }

        std::cout << "total area:" << total_area << '\n';
    }

   private:
    std::vector<Packet> packets;

    struct PairHash {
        size_t operator()(const std::pair<size_t, size_t>& pair) const {
            return pair.first * 32 + pair.second;
        }
    };
};

#endif