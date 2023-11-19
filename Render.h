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
            b.resize(static_cast<size_t>(static_cast<double>(width_) * scale));
        }
    }
    void AddRectangle(const RectWithPos& rect_with_pos) {
        size_t down_row = rect_with_pos.pos.x + rect_with_pos.rect->height - 1;
        size_t right_col = rect_with_pos.pos.y + rect_with_pos.rect->width - 1;
        if (buf[0].size() <= right_col) {
            for (auto& b : buf) {
                b.resize(right_col + 1, ' ');
            }
        }
        buf[rect_with_pos.pos.x][rect_with_pos.pos.y] = '+';
        for (size_t i = 1; i + 1 < rect_with_pos.rect->width; ++i) {
            buf[rect_with_pos.pos.x][rect_with_pos.pos.y + i] = '-';
            buf[down_row][rect_with_pos.pos.y + i] = '-';
        }

        buf[rect_with_pos.pos.x][right_col] = '+';
        buf[down_row][rect_with_pos.pos.y] = '+';
        buf[down_row][right_col] = '+';
        for (size_t i = 1; i + 1 < rect_with_pos.rect->height; ++i) {
            buf[rect_with_pos.pos.x + i][rect_with_pos.pos.y] = '|';
            buf[rect_with_pos.pos.x + i][right_col] = '|';
        }
    }
    void Draw(const Schedule& schedule) {
        Clear();
        for (const auto& packet : schedule.packets) {
            for (const auto rect_with_pos : packet.rectangles) {
                AddRectangle(rect_with_pos);
            }
        }

        for (size_t i = 0; i < buf.size(); ++i) {
            std::cout << std::setw(3) << i << '#';
            for (size_t j = 0; j < buf[i].size(); ++j) {
                std::cout << (j == width_ && buf[i][j] == ' ' ? '#' : buf[i][j]);
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