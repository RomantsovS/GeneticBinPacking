#include <cassert>
#include <iostream>
#include <limits>

#include "GA.h"
#include "Rectangle.h"
#include "Render.h"
#include "Schedule.h"

int main(int argc, char* argv[]) {
    const size_t num_parameters = 8;
    if (argc != num_parameters) {
        std::cerr << "incorrect parameters\n";
        return 1;
    }

    unsigned rand_seed = std::stoi(argv[1]) ? time(nullptr) : 0;
    srand(rand_seed);

    const size_t total_width = 60;
    const double scale = 1.5;
    const size_t num_packets = std::stoi(argv[2]);
    const size_t max_rectangles = std::stoi(argv[3]);
    const size_t max_rectalgle_height = std::stoi(argv[4]);
    const size_t max_attemptions = 10000;
    const size_t max_packet_rectangles = std::stoi(argv[5]);

    std::vector<Rectangle> rectangles;
    rectangles.reserve(max_rectangles);

    Schedule schedule;
    for (size_t i = 0; i < num_packets; ++i) {
        schedule.addPacket({i});
    }

    for (size_t i = 0; i < max_rectangles; ++i) {
        size_t packet_id = std::numeric_limits<size_t>::max();
        int iteration = 0;
        while (packet_id == std::numeric_limits<size_t>::max() ||
               schedule.getPackets()[packet_id].rectangles.size() == max_packet_rectangles) {
            packet_id = rand() % num_packets;
            if (iteration == max_attemptions) {
                std::cerr << "can't find packet for allocation\n";
                return -1;
            }
        }

        size_t packet_width = schedule.getPackets()[packet_id].width();

        for (size_t j = 0; j < max_attemptions; ++j) {
            size_t rect_height =
                std::min(1 + std::min(rand() % (num_packets / 4), num_packets - packet_id - 1),
                         max_rectalgle_height);
            size_t rect_width =
                1 + rand() % (std::min(total_width / 3, total_width - packet_width));
            Rectangle rect(rectangles.size(), rect_height, rect_width);
            RectWithPos rect_with_pos{
                &rect, Pos{packet_id,
                           rand() % (static_cast<size_t>(static_cast<double>(total_width) * scale) -
                                     rect_width)}};
            if (schedule.hasIntersection(rect_with_pos)) {
                continue;
            }
            rectangles.push_back(rect);
            rect_with_pos.rect = &rectangles.back();
            schedule.addRectangle(packet_id, rect_with_pos);

            break;
        }
        if (rectangles.size() == max_rectangles) {
            break;
        }
    }

    schedule.print();

    if (rectangles.size() != max_rectangles) {
        std::cerr << "allocated only " << rectangles.size() << "/" << max_rectangles << '\n';
        return 1;
    }

    Renderer renderer(num_packets, total_width);
    renderer.Draw(schedule);

    std::flush(std::cout);

    const size_t max_iterations = std::stoi(argv[6]);
    const size_t max_population = 25;
    GA genetic_alg(total_width, max_iterations, max_population);

    auto new_schedules = genetic_alg.Solve(schedule, std::stoi(argv[7]), rand_seed);

    for (size_t i = 0; i < new_schedules.size(); ++i) {
        std::cout << "schedule: " << i << '\n';
        renderer.Draw(new_schedules[i]);
    }

    return 0;
}
