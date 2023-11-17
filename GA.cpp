#include "GA.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>

GA::GA(size_t width, const Schedule& schedule, size_t max_iterations, size_t max_population)
    : width_(width),
      schedule_(&schedule),
      max_iterations_(max_iterations),
      max_population_(max_population) {
    populations_.reserve(max_population_);
}

std::vector<Schedule> GA::Solve() {
    double fitness = -1;

    std::vector<RectWithPacket> rectangles_order = getRectanglesOrder(*schedule_);

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

    for (int i = 0; i < max_population_; i++) {
        Gene gene;
        gene.rectangles_order = SwapRectanglesOrder(new_schedule, rectangles_order);
        gene.fitness = Fitness(gene);

        std::cout << i << " fit: " << gene.fitness << '\n';

        res.push_back(makeSchedule(gene.rectangles_order));

        populations_.push_back(std::move(gene));
    }

    return res;

    int iterations = 0;
    while (iterations < max_iterations_) {
        std::cout << "iter: " << iterations << '\n';
        GenerateLikelihoods();
        CreateChilds();
        break;

        iterations++;
    }

    return {};
}

Schedule GA::makeSchedule(const std::vector<RectWithPacket>& rectangles_order) {
    Schedule result_schedule;
    for (size_t i = 0; i < schedule_->packets.size(); ++i) {
        result_schedule.packets.emplace_back(i);
    }

    const size_t width_scale = 2;
    for (const auto rect_order : rectangles_order) {
        auto rect_with_pos = rect_order.rect_with_pos;
        rect_with_pos.pos.y = width_ * width_scale;
        while (rect_with_pos.pos.y > 0 && !result_schedule.hasIntersection(rect_with_pos)) {
            --rect_with_pos.pos.y;
        }
        ++rect_with_pos.pos.y;
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
    const Schedule& schedule, const std::vector<RectWithPacket>& rectangles_order) {
    std::vector<RectWithPacket> result = rectangles_order;

    size_t packet_id = rand() % schedule.packets.size();
    while (schedule.packets[packet_id].rectangles.size() < 2) {
        packet_id = rand() % schedule.packets.size();
    }
    const auto& packet_rectangles = schedule.packets[packet_id].rectangles;
    size_t first_rect_id = rand() % packet_rectangles.size();
    size_t second_rect_id = rand() % packet_rectangles.size();
    while (first_rect_id == second_rect_id) {
        second_rect_id = rand() % packet_rectangles.size();
    }
    const auto first_rect_iter = std::find_if(
        result.begin(), result.end(),
        [rect_with_pos = packet_rectangles.at(first_rect_id)](const RectWithPacket rect_packet) {
            return rect_packet.rect_with_pos.rect == rect_with_pos.rect;
        });
    const auto second_rect_iter = std::find_if(
        result.begin(), result.end(),
        [rect_with_pos = packet_rectangles.at(second_rect_id)](const RectWithPacket rect_packet) {
            return rect_packet.rect_with_pos.rect == rect_with_pos.rect;
        });

    std::cout << "swap in pack " << packet_id << ": " << *first_rect_iter->rect_with_pos.rect
              << " and " << *second_rect_iter->rect_with_pos.rect << '\n';
    std::iter_swap(first_rect_iter, second_rect_iter);

    return result;
}

double GA::Fitness(const Gene& gene) {
    const auto gene_schedule = makeSchedule(gene.rectangles_order);
    return static_cast<double>(gene_schedule.OutOfRangeSize(width_));
}

void GA::GenerateLikelihoods() {
    double multinv = MultInv();

    double last = 0;
    const double one_hundred_percent = 100;
    for (int i = 0; i < max_population_; i++) {
        populations_[i].likelihood = last =
            last + ((1 / (populations_[i].fitness) / multinv) * one_hundred_percent);
        std::cout << "% for " << i << " : " << last << '\n';
    }
}

double GA::MultInv() {
    double sum = 0;

    for (int i = 0; i < max_population_; i++) {
        sum += 1 / (populations_[i].fitness);
    }

    return sum;
}

void GA::CreateChilds() {
    std::vector<Gene> temppop(max_population_);

    for (int i = 0; i < max_population_; i++) {
        int parent1 = 0, parent2 = 0, iterations = 0;
        while (parent1 == parent2 || populations_[parent1] == populations_[parent2]) {
            parent1 = GetIndex(static_cast<double>(rand() % 101));
            parent2 = GetIndex(static_cast<double>(rand() % 101));
        }

        temppop[i] = Crossover(parent1, parent2);  // Create a child.
    }

    populations_ = std::move(temppop);
}

int GA::GetIndex(double val) {
    double last = 0;
    for (int i = 0; i < max_population_; i++) {
        if (last <= val && val <= populations_[i].likelihood)
            return i;
        else
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

    auto rectangles_order = populations_[p2].rectangles_order;
    auto end_iter = std::remove_if(
        rectangles_order.begin(), rectangles_order.end(),
        [&child, crossover](RectWithPacket rect_with_packet) {
            return std::find_if(child.rectangles_order.begin(),
                                child.rectangles_order.begin() + crossover,
                                [rect_with_packet](RectWithPacket rect_with_packet_child) {
                                    return rect_with_packet.rect_with_pos.rect ==
                                           rect_with_packet_child.rect_with_pos.rect;
                                }) != child.rectangles_order.end();
        });

    std::copy(rectangles_order.begin(), end_iter, child.rectangles_order.begin() + crossover);

    std::cout << "crossover: " << p1 << " and " << p2 << '\n';

    return child;
}