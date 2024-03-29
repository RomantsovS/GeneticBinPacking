#include <cassert>
#include <iostream>
#include <limits>

#include "GA.h"
#include "Rectangle.h"
#include "Render.h"
#include "Schedule.h"

int main(int argc, char* argv[]) {
    const size_t num_parameters = 11;
    if (argc != num_parameters) {
        std::cerr << "incorrect parameters\n";
        return 1;
    }

    unsigned rand_seed = std::stoi(argv[1]) ? time(nullptr) : 0;
    srand(rand_seed);

    const size_t total_width = std::stoi(argv[2]);
    const double scale = 1.5;
    const size_t num_steps = std::stoi(argv[3]);
    const size_t max_rectangles = std::stoi(argv[4]);
    const size_t max_rectalgle_height = std::stoi(argv[5]);
    const size_t max_rectalgle_width = std::stoi(argv[6]);
    const size_t max_attemptions = 10000;
    const size_t max_Step_rectangles = std::stoi(argv[7]);

    std::vector<Rectangle> rectangles;
    rectangles.reserve(max_rectangles);

    std::cout << "total available area:" << (num_steps * total_width) << '\n';

    Schedule schedule(num_steps);

    for (size_t i = 0; i < max_rectangles; ++i) {
        size_t Step_id = std::numeric_limits<size_t>::max();
        size_t Step_width;
        int iteration = 0;
        while (Step_id == std::numeric_limits<size_t>::max() ||
               schedule.getSteps()[Step_id].rectangles.size() == max_Step_rectangles ||
               (Step_width = schedule.getSteps()[Step_id].width()) + 1 >= total_width) {
            Step_id = rand() % num_steps;
            if (++iteration == max_attemptions) {
                std::cerr << "can't find Step for allocation\n";
                return -1;
            }
        }

        for (size_t j = 0; j < max_attemptions; ++j) {
            size_t rect_height =
                std::min(1 + std::min(rand() % max_rectalgle_height, num_steps - Step_id - 1),
                         max_rectalgle_height);
            size_t rect_width =
                std::min(1 + std::min(rand() % max_rectalgle_width, total_width - Step_width),
                         max_rectalgle_width);
            Rectangle rect(rectangles.size(), rect_height, rect_width);
            RectWithPos rect_with_pos{
                &rect, Pos{Step_id,
                           rand() % (static_cast<size_t>(static_cast<double>(total_width) * scale) -
                                     rect_width)}};
            if (schedule.hasIntersection(rect_with_pos)) {
                continue;
            }
            rectangles.push_back(rect);
            rect_with_pos.rect = &rectangles.back();
            schedule.addRectangle(Step_id, rect_with_pos);

            break;
        }
        if (rectangles.size() == max_rectangles) {
            break;
        }
    }

    // schedule.print();

    if (rectangles.size() != max_rectangles) {
        std::cerr << "allocated only " << rectangles.size() << "/" << max_rectangles << '\n';
        return 1;
    }

    Renderer renderer(num_steps, total_width);
    renderer.Draw(schedule);

    std::flush(std::cout);

    const size_t max_iterations = std::stoi(argv[8]);
    const size_t max_population = std::stoi(argv[9]);
    GA genetic_alg(total_width, max_iterations, max_population);

    auto new_schedules = genetic_alg.Solve(schedule, std::stoi(argv[10]), rand_seed);

    for (size_t i = 0; i < new_schedules.size(); ++i) {
        std::cout << "schedule: " << i << '\n';
        renderer.Draw(new_schedules[i]);
    }

    genetic_alg.PrintStatistic();

    return 0;
}
