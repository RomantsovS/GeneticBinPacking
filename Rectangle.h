#ifndef RECTANGLE_H
#define RECTANGLE_H

struct Pos {
    size_t x, y;
};

// bool operator==(Pos lhs, Pos rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
// bool operator!=(Pos lhs, Pos rhs) { return !(lhs == rhs); }

struct Rectangle {
    Rectangle(size_t id, Pos pos, size_t height, size_t width)
        : id(id), pos(pos), height(height), width(width) {}
    bool intersect(const Rectangle& other) const {
        return !(pos.y > other.pos.y + other.width || pos.y + width < other.pos.y ||
                 pos.x > other.pos.x + other.height || pos.x + height < other.pos.x);
    }

    size_t id;
    Pos pos;
    size_t height, width;
};

#endif