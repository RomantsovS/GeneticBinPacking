#include <iostream>

#include "GA.h"
#include "Rectangle.h"
#include "Render.h"
#include "Schedule.h"

int main() {
    // srand(time(nullptr));

    const size_t total_width = 60;
    const double scale = 1.5;
    Schedule schedule;
    const size_t num_packets = 20;
    const size_t max_rectangles = 20;
    const size_t max_attemptions = 100;

    std::vector<Rectangle> rectangles;
    rectangles.reserve(max_rectangles);

    for (size_t i = 0; i < num_packets; ++i) {
        schedule.packets.emplace_back(i);
        std::cout << "Packet #" << i << '\n';
        for (int j = 0; j < max_attemptions; ++j) {
            size_t rect_height = 1 + std::min(rand() % (num_packets / 2), num_packets - i - 2);
            size_t rect_width = 1 + rand() % (total_width / 2);
            Rectangle rect(rectangles.size(), rect_height, rect_width);
            RectWithPos rect_with_pos{
                &rect,
                Pos{i, rand() % (static_cast<size_t>(static_cast<double>(total_width) * scale) -
                                 rect_width)}};
            if (schedule.hasIntersection(rect_with_pos)) {
                continue;
            }
            rectangles.push_back(std::move(rect));
            rect_with_pos.rect = &rectangles.back();
            schedule.packets.back().rectangles.push_back(rect_with_pos);
            std::cout << "\t" << schedule.packets.back().rectangles.back() << '\n';
            if (rectangles.size() == max_rectangles ||
                schedule.packets.back().rectangles.size() > (rand() % 4)) {
                break;
            }
        }
        if (rectangles.size() == max_rectangles) {
            break;
        }
    }

    Renderer renderer(num_packets, total_width, scale);
    renderer.Draw(schedule);

    const size_t max_iterations = 10;
    const size_t max_population = 25;
    GA ga(total_width, schedule, max_iterations, max_population);

    auto new_schedule = ga.Solve();

    renderer.Draw(new_schedule);

    return 0;
}
