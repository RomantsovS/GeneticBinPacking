#ifndef RENDERER_H
#define RENDERER_H

#include <iomanip>
#include <iostream>
#include <vector>

#include "Schedule.h"

class Renderer {
   public:
    Renderer(size_t height, size_t width, double scale)
        : height_(height), width_(width), scale_(scale) {
        buf.resize(height_);
        for (auto& b : buf) {
            b.resize(width_ * scale);
        }
    }
    void AddRectangle(const Rectangle& r) {
        buf[r.pos.x][r.pos.y] = '+';
        for (int i = 1; i < r.width; ++i) {
            buf[r.pos.x][r.pos.y + i] = '-';
        }
        buf[r.pos.x][r.pos.y + r.width] = '+';
        buf[r.pos.x + r.height][r.pos.y] = '+';
        for (int i = 1; i < r.width; ++i) {
            buf[r.pos.x + r.height][r.pos.y + i] = '-';
        }
        buf[r.pos.x + r.height][r.pos.y + r.width] = '+';
        for (int i = 1; i < r.height; ++i) {
            buf[r.pos.x + i][r.pos.y] = '|';
            buf[r.pos.x + i][r.pos.y + r.width] = '|';
        }
    }
    void Draw(const Schedule& schedule) {
        Clear();
        for (const auto& packet : schedule.packets) {
            for (const auto* rect : packet.rectangles) {
                AddRectangle(*rect);
            }
        }

        for (size_t i = 0; i < buf.size(); ++i) {
            std::cout << std::setw(3) << i << '#';
            for (int j = 0; j < buf[i].size(); ++j) {
                std::cout << (j == width_ ? '#' : buf[i][j]);
            }
            std::cout << '#' << i << '\n';
        }
    }

   private:
    void Clear() {
        for (auto& b : buf) {
            std::fill(b.begin(), b.end(), ' ');
        }
    }
    size_t height_;
    size_t width_;
    double scale_;
    std::vector<std::vector<char>> buf;
};

#endif