#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

#include "GA.h"
#include "Rectangle.h"
#include "Schedule.h"

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
    void Draw(const Schedule& schedule) {
        Clear();
        for (const auto& packet : schedule.packets) {
            for (const auto& r : packet.rectangles) {
                AddRectangle(r);
            }
        }

        for (size_t i = 0; i < buf.size(); ++i) {
            std::cout << std::setw(3) << i << '#';
            for (auto ch : buf[i]) {
                std::cout << ch;
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
    int height_;
    int width_;
    std::vector<std::vector<char>> buf;
};

int main() {
    // srand(time(nullptr));

    int width = 60;
    Schedule schedule;
    int num_packets = 20;

    for (int i = 0; i < num_packets; ++i) {
        schedule.packets.emplace_back();
        std::cout << "Packet: " << i << '\n';
        for (int j = 0; j < 100; ++j) {
            int h = 1 + std::min(rand() % (num_packets / 2), num_packets - i - 2);
            int w = 1 + rand() % (width / 2);
            Rectangle r({i, rand() % (width - w)}, h, w);
            if (std::any_of(schedule.packets.begin(), schedule.packets.end(),
                            [&r](const Packet& packet) {
                                return std::any_of(
                                    packet.rectangles.begin(), packet.rectangles.end(),
                                    [&r](const Rectangle& other) { return r.intersect(other); });
                            })) {
                continue;
            }
            schedule.packets.back().rectangles.push_back(r);
            std::cout << "\t" << r.pos.y << '\n';
            if (schedule.packets.size() > (rand() % 4)) {
                break;
            }
        }
    }

    Renderer renderer(num_packets, width);
    renderer.Draw(schedule);

    // GA dp(width, rectangles);

    // int ans;
    // ans = dp.Solve();
    // if (ans == -1) {
    //     std::cout << "No solution found." << std::endl;
    // } else {
    //     gene gn = dp.GetGene(ans);

    //     std::cout << "The solution set to a+2b+3c+4d=30 is:\n";
    //     std::cout << "a = " << gn.alleles[0] << "." << std::endl;
    //     std::cout << "b = " << gn.alleles[1] << "." << std::endl;
    //     std::cout << "c = " << gn.alleles[2] << "." << std::endl;
    //     std::cout << "d = " << gn.alleles[3] << "." << std::endl;
    // }

    return 0;
}
