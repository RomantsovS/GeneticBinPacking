#include <iostream>

#include "GA.h"
#include "Rectangle.h"
#include "Render.h"
#include "Schedule.h"

int main() {
    // srand(time(nullptr));

    const size_t width = 60;
    const double scale = 1.5;
    Schedule schedule;
    const size_t num_packets = 20;
    const size_t max_rectangles = 20;
    const size_t max_attemptions = 100;

    for (size_t i = 0; i < num_packets; ++i) {
        schedule.packets.emplace_back(i);
        std::cout << "Packet #" << i << '\n';
        for (int j = 0; j < max_attemptions; ++j) {
            size_t h = 1 + std::min(rand() % (num_packets / 2), num_packets - i - 2);
            size_t w = 1 + rand() % (width / 2);
            auto r = std::make_unique<Rectangle>(
                schedule.rectangles.size(),
                Pos{i, rand() % (static_cast<size_t>(static_cast<double>(width) * scale) - w)}, h,
                w);
            if (schedule.hasIntersection(*r)) {
                continue;
            }
            schedule.rectangles.push_back(std::move(r));
            schedule.packets.back().rectangles.push_back(schedule.rectangles.back().get());
            std::cout << "\t" << schedule.packets.back().rectangles.back()->pos.y << '\n';
            if (schedule.rectangles.size() == max_rectangles ||
                schedule.packets.back().rectangles.size() > (rand() % 4)) {
                break;
            }
        }
        if (schedule.rectangles.size() == max_rectangles) {
            break;
        }
    }

    Renderer renderer(num_packets, width, scale);
    renderer.Draw(schedule);

    const size_t max_iterations = 10;
    const size_t max_population = 25;
    GA ga(width, schedule, max_iterations, max_population);

    auto new_schedule = ga.Solve();

    renderer.Draw(new_schedule);

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
