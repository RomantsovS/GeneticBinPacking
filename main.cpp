#include <cassert>
#include <iostream>

#include "GA.h"
#include "Rectangle.h"
#include "Render.h"
#include "Schedule.h"

int main(int argc, char* argv[]) {
    const size_t num_parameters = 7;
    if (argc != num_parameters) {
        std::cerr << "incorrect parameters\n";
        return 1;
    }
    // srand(time(nullptr));

    const size_t total_width = 60;
    const double scale = 1.5;
    Schedule schedule;
    const size_t num_packets = std::stoi(argv[1]);
    const size_t max_rectangles = std::stoi(argv[2]);
    const size_t max_rectalgle_height = std::stoi(argv[3]);
    const size_t max_attemptions = 10000;
    const size_t max_packet_rectangles = std::stoi(argv[4]);

    std::vector<Rectangle> rectangles;
    rectangles.reserve(max_rectangles);

    for (size_t i = 0; i < num_packets; ++i) {
        schedule.packets.emplace_back(i);
        std::cout << "Packet #" << i << '\n';
        for (size_t j = 0; j < max_attemptions; ++j) {
            size_t rect_height =
                std::min(1 + std::min(rand() % (num_packets / 4), num_packets - i - 1),
                         max_rectalgle_height);
            size_t rect_width = 1 + rand() % (total_width / 2);
            Rectangle rect(rectangles.size(), rect_height, rect_width);
            RectWithPos rect_with_pos{
                &rect,
                Pos{i, rand() % (static_cast<size_t>(static_cast<double>(total_width) * scale) -
                                 rect_width)}};
            if (schedule.hasIntersection(rect_with_pos)) {
                continue;
            }
            rectangles.push_back(rect);
            rect_with_pos.rect = &rectangles.back();
            schedule.packets.back().rectangles.push_back(rect_with_pos);
            std::cout << "\t" << schedule.packets.back().rectangles.back() << '\n';
            if (rectangles.size() == max_rectangles ||
                schedule.packets.back().rectangles.size() > (rand() % max_packet_rectangles)) {
                break;
            }
        }
        if (rectangles.size() == max_rectangles) {
            break;
        }
    }

    if (rectangles.size() != max_rectangles) {
        std::cerr << "allocated only " << rectangles.size() << "/" << max_rectangles << '\n';
        return 1;
    }

    Renderer renderer(num_packets, total_width, scale);
    renderer.Draw(schedule);

    std::flush(std::cout);

    const size_t max_iterations = std::stoi(argv[5]);
    const size_t max_population = 25;
    GA genetic_alg(total_width, max_iterations, max_population);

    auto new_schedules = genetic_alg.Solve(schedule, std::stoi(argv[6]));

    for (size_t i = 0; i < new_schedules.size(); ++i) {
        std::cout << "schedule: " << i << '\n';
        renderer.Draw(new_schedules[i]);
    }

    return 0;
}
