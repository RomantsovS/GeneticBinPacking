#ifndef RECTANGLE_H
#define RECTANGLE_H

struct Rectangle {
    Rectangle(size_t i, size_t h, size_t w) : id(i), height(h), width(w) {}

    size_t id;
    size_t height, width;
};

inline std::ostream& operator<<(std::ostream& os, Rectangle rect) {
    return os << "rect " << rect.id << ": h:" << rect.height << " w:" << rect.width;
}

#endif