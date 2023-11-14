#include <algorithm>
#include <iostream>
#include <vector>

struct Rectangle {
    Rectangle(int x, int y, int height, int width) : x_(x), y_(y), height_(height), width_(width) {}
    bool intersect(const Rectangle& other) {
        return !(y_ > other.y_ + other.width_ || y_ + width_ < other.y_ ||
                 x_ > other.x_ + other.height_ || x_ + height_ < other.x_);
    }
    int x_, y_;
    int height_, width_;
};

class Renderer {
   public:
    Renderer(int height, int width) : height_(height), width_(width) {
        buf.resize(height_);
        for (auto& b : buf) {
            b.resize(width_);
        }
    }
    void AddRectangle(const Rectangle& r) {
        buf[r.x_][r.y_] = '+';
        for (int i = 1; i < r.width_; ++i) {
            buf[r.x_][r.y_ + i] = '-';
        }
        buf[r.x_][r.y_ + r.width_] = '+';
        buf[r.x_ + r.height_][r.y_] = '+';
        for (int i = 1; i < r.width_; ++i) {
            buf[r.x_ + r.height_][r.y_ + i] = '-';
        }
        buf[r.x_ + r.height_][r.y_ + r.width_] = '+';
        for (int i = 1; i < r.height_; ++i) {
            buf[r.x_ + i][r.y_] = '|';
            buf[r.x_ + i][r.y_ + r.width_] = '|';
        }
    }
    void Draw(const std::vector<Rectangle>& rectangles) {
        Clear();
        for (const auto& r : rectangles) {
            AddRectangle(r);
        }

        for (auto& b : buf) {
            std::cout << '#';
            for (auto ch : b) {
                std::cout << ch;
            }
            std::cout << '#' << '\n';
        }
    }

   private:
    void Clear() {
        for (auto& b : buf) {
            std::fill(b.begin(), b.end(), ' ');
        }
    }
    int height_;
    int width_;
    std::vector<std::vector<char>> buf;
};

int main() {
    // srand(time(nullptr));

    int height = 32;
    int width = 40;

    std::vector<Rectangle> rectangles;
    for (size_t i = 0; i < 20; ++i) {
        for (int j = 0; j < 100; ++j) {
            int h = 1 + rand() % 20;
            int w = 1 + rand() % 20;
            Rectangle r(rand() % (height - h), rand() % (width - w), h, w);
            if (std::any_of(rectangles.begin(), rectangles.end(),
                            [&r](const Rectangle& other) { return r.intersect(other); })) {
                continue;
            }
            rectangles.push_back(r);
            break;
        }
    }

    Renderer renderer(height, width);
    renderer.Draw(rectangles);

    std::cout << rectangles.size() << '\n';

    return 0;
}
