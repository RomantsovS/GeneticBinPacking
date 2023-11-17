#include "GA.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <limits>

GA::GA(size_t width, size_t max_iterations, size_t max_population)
    : width_(width), max_iterations_(max_iterations), max_population_(max_population) {
    populations_.reserve(max_population_);
}

std::vector<Schedule> GA::Solve(const Schedule& schedule) {
    std::vector<RectWithPacket> rectangles_order = getRectanglesOrder(schedule);

    auto new_schedule = makeSchedule(rectangles_order);

    auto s = new_schedule.OutOfRangeSize(width_);
    std::cout << " fit: " << s << '\n';
    if (s == 0) {
        return {new_schedule};
    }

    // Generate initial population.
    // srand((unsigned)time(nullptr));

    std::vector<Schedule> res;
    res.push_back(new_schedule);

    for (size_t i = 0; i < max_population_; i++) {
        Gene gene;
        gene.rectangles_order = SwapRectanglesOrder(rectangles_order);
        populations_.push_back(std::move(gene));
    }

    CalculateFitnesses(res);

    size_t iterations = 0;
    while (iterations < max_iterations_) {
        std::cout << "iter: " << iterations << '\n';
        GenerateLikelihoods();
        CreateChilds();
        CalculateFitnesses(res);

        ++iterations;
    }

    return res;
}

Schedule GA::makeSchedule(const std::vector<RectWithPacket>& rectangles_order) {
    Schedule result_schedule;

    const size_t width_scale = 2;
    for (const auto rect_order : rectangles_order) {
        auto rect_with_pos = rect_order.rect_with_pos;
        rect_with_pos.pos.y = width_ * width_scale;
        while (rect_with_pos.pos.y > 0 && !result_schedule.hasIntersection(rect_with_pos)) {
            --rect_with_pos.pos.y;
        }
        ++rect_with_pos.pos.y;
        if (result_schedule.packets.size() <= rect_order.packet->id) {
            result_schedule.packets.reserve(rect_order.packet->id);
            while (result_schedule.packets.size() <= rect_order.packet->id) {
                result_schedule.packets.emplace_back(result_schedule.packets.size());
            }
        }
        result_schedule.packets[rect_order.packet->id].rectangles.push_back(rect_with_pos);
    }

    return result_schedule;
}

std::vector<RectWithPacket> GA::getRectanglesOrder(const Schedule& schedule) {
    std::vector<RectWithPacket> rectangles_order;

    for (const auto& packet : schedule.packets) {
        for (const auto rect_with_pos : packet.rectangles) {
            rectangles_order.push_back(RectWithPacket{rect_with_pos, &packet});
        }
    }

    std::sort(rectangles_order.begin(), rectangles_order.end(),
              [](const RectWithPacket lhs, const RectWithPacket rhs) {
                  return lhs.rect_with_pos.pos.y == rhs.rect_with_pos.pos.y
                             ? lhs.packet->id < rhs.packet->id
                             : lhs.rect_with_pos.pos.y < rhs.rect_with_pos.pos.y;
              });

    return rectangles_order;
}

std::vector<RectWithPacket> GA::SwapRectanglesOrder(
    const std::vector<RectWithPacket>& rectangles_order) {
    std::vector<RectWithPacket> result = rectangles_order;

    // size_t packet_id = rand() % schedule.packets.size();
    // while (schedule.packets[packet_id].rectangles.size() < 2) {
    //     packet_id = rand() % schedule.packets.size();
    // }
    // const auto& packet_rectangles = schedule.packets[packet_id].rectangles;
    // size_t first_rect_id = rand() % packet_rectangles.size();
    // size_t second_rect_id = rand() % packet_rectangles.size();
    // while (first_rect_id == second_rect_id) {
    //     second_rect_id = rand() % packet_rectangles.size();
    // }
    // const auto first_rect_iter = std::find_if(
    //     result.begin(), result.end(),
    //     [rect_with_pos = packet_rectangles.at(first_rect_id)](const RectWithPacket rect_packet) {
    //         return rect_packet.rect_with_pos.rect == rect_with_pos.rect;
    //     });
    // const auto second_rect_iter = std::find_if(
    //     result.begin(), result.end(),
    //     [rect_with_pos = packet_rectangles.at(second_rect_id)](const RectWithPacket rect_packet)
    //     {
    //         return rect_packet.rect_with_pos.rect == rect_with_pos.rect;
    //     });

    // std::cout << "swap in pack " << packet_id << ": " << *first_rect_iter->rect_with_pos.rect
    //           << " and " << *second_rect_iter->rect_with_pos.rect << '\n';
    // std::iter_swap(first_rect_iter, second_rect_iter);
    size_t first_rect_id = rand() % result.size();
    size_t second_rect_id = rand() % result.size();
    while (first_rect_id == second_rect_id) {
        second_rect_id = rand() % result.size();
    }
    std::cout << "swap : " << *(result.begin() + first_rect_id)->rect_with_pos.rect << " and "
              << *(result.begin() + second_rect_id)->rect_with_pos.rect << '\n';
    std::iter_swap(result.begin() + first_rect_id, result.begin() + second_rect_id);

    return result;
}

void GA::CalculateFitnesses(std::vector<Schedule>& res) {
    double avg_fit = 0;
    double min_fit = std::numeric_limits<double>::max();

    res.clear();

    for (size_t i = 0; i < populations_.size(); ++i) {
        auto& gene = populations_[i];
        auto schedule = makeSchedule(gene.rectangles_order);
        gene.fitness = Fitness(schedule);

        avg_fit += gene.fitness;
        min_fit = std::min(min_fit, gene.fitness);

        std::cout << i << " fit: " << gene.fitness << '\n';

        res.push_back(schedule);
    }

    avg_fit /= populations_.size();
    std::cout << "avg fit: " << avg_fit << ", min fit: " << min_fit << '\n';
}

double GA::Fitness(const Schedule& schedule) {
    return static_cast<double>(schedule.OutOfRangeSize(width_));
}

void GA::GenerateLikelihoods() {
    double multinv = MultInv();

    double last = 0;
    const double one_hundred_percent = 100;
    for (size_t i = 0; i < max_population_; i++) {
        populations_[i].likelihood = last =
            last + ((1 / (populations_[i].fitness) / multinv) * one_hundred_percent);
        // std::cout << "% for " << i << " : " << last << '\n';
    }
}

double GA::MultInv() {
    double sum = 0;

    for (size_t i = 0; i < max_population_; i++) {
        sum += 1 / (populations_[i].fitness);
    }

    return sum;
}

void GA::CreateChilds() {
    std::vector<Gene> temppop(max_population_);

    for (size_t i = 0; i < max_population_; i++) {
        size_t parent1 = 0;
        size_t parent2 = 0;
        size_t iteration = 0;
        const size_t max_iterations = 25;
        while (parent1 == parent2) {
            parent1 = GetIndex(static_cast<double>(rand() % 101));
            parent2 = GetIndex(static_cast<double>(rand() % 101));

            if (parent1 != parent2 && populations_[parent1] == populations_[parent2]) {
                std::cout << "equal genes for " << parent1 << " and " << parent2 << '\n';
                parent1 = parent2;
                continue;
            }
            if (++iteration == max_iterations) {
                break;
            }
        }

        temppop[i] = Crossover(parent1, parent2);  // Create a child.
    }

    populations_ = std::move(temppop);
}

size_t GA::GetIndex(double val) {
    double last = 0;
    for (size_t i = 0; i < max_population_; i++) {
        if (last <= val && val <= populations_[i].likelihood) {
            return i;
        }

        last = populations_[i].likelihood;
    }

    return max_population_ - 1;
}

Gene GA::Crossover(int p1, int p2) {
    // Which parent comes first?
    if (rand() % 2) {
        std::swap(p1, p2);
    }

    auto child = populations_[p1];  // Child is all first parent initially.

    size_t crossover =
        rand() % child.rectangles_order.size() + 1;  // Create the crossover point (not first).

    std::cout << "crossover: " << p1 << " and " << p2 << " with " << crossover << '\n';

    auto rectangles_order = populations_[p2].rectangles_order;
    auto end_iter = std::remove_if(
        rectangles_order.begin(), rectangles_order.end(),
        [&child, crossover](RectWithPacket rect_with_packet) {
            return std::find_if(child.rectangles_order.begin(),
                                child.rectangles_order.begin() + crossover,
                                [rect_with_packet](RectWithPacket rect_with_packet_child) {
                                    return rect_with_packet.rect_with_pos.rect ==
                                           rect_with_packet_child.rect_with_pos.rect;
                                }) != child.rectangles_order.begin() + crossover;
        });

    assert(end_iter - rectangles_order.begin() ==
           static_cast<int>(rectangles_order.size() - crossover));

    std::copy(rectangles_order.begin(), end_iter, child.rectangles_order.begin() + crossover);

    if (rand() % 10 == 0) {
        child.rectangles_order = SwapRectanglesOrder(child.rectangles_order);
    }

    return child;
}