#ifndef RECTANGLE_H
#define RECTANGLE_H

struct Pos {
    int x, y;
};

// bool operator==(Pos lhs, Pos rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
// bool operator!=(Pos lhs, Pos rhs) { return !(lhs == rhs); }

struct Rectangle {
    Rectangle(Pos pos, int height, int width) : pos(pos), height_(height), width_(width) {}
    bool intersect(const Rectangle& other) {
        return !(pos.y > other.pos.y + other.width_ || pos.y + width_ < other.pos.y ||
                 pos.x > other.pos.x + other.height_ || pos.x + height_ < other.pos.x);
    }
    Pos pos;
    int height_, width_;
};

#endif