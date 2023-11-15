#include <algorithm>
#include <iostream>
#include <vector>

#include "GA.h"
#include "Rectangle.h"

class Renderer {
   public:
    Renderer(int height, int width) : height_(height), width_(width) {
        buf.resize(height_);
        for (auto& b : buf) {
            b.resize(width_);
        }
    }
    void AddRectangle(const Rectangle& r) {
        buf[r.pos.x][r.pos.y] = '+';
        for (int i = 1; i < r.width_; ++i) {
            buf[r.pos.x][r.pos.y + i] = '-';
        }
        buf[r.pos.x][r.pos.y + r.width_] = '+';
        buf[r.pos.x + r.height_][r.pos.y] = '+';
        for (int i = 1; i < r.width_; ++i) {
            buf[r.pos.x + r.height_][r.pos.y + i] = '-';
        }
        buf[r.pos.x + r.height_][r.pos.y + r.width_] = '+';
        for (int i = 1; i < r.height_; ++i) {
            buf[r.pos.x + i][r.pos.y] = '|';
            buf[r.pos.x + i][r.pos.y + r.width_] = '|';
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
            Rectangle r({rand() % (height - h), rand() % (width - w)}, h, w);
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

    GA dp(width, rectangles);

    int ans;
    ans = dp.Solve();
    if (ans == -1) {
        std::cout << "No solution found." << std::endl;
    } else {
        gene gn = dp.GetGene(ans);

        std::cout << "The solution set to a+2b+3c+4d=30 is:\n";
        std::cout << "a = " << gn.alleles[0] << "." << std::endl;
        std::cout << "b = " << gn.alleles[1] << "." << std::endl;
        std::cout << "c = " << gn.alleles[2] << "." << std::endl;
        std::cout << "d = " << gn.alleles[3] << "." << std::endl;
    }

    return 0;
}
