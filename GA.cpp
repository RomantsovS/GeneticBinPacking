#include "GA.h"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>

bool operator==(const Gene& lhs, const Gene& rhs) {
    return lhs.rectangles_order == rhs.rectangles_order;
}
bool operator<(const Gene& lhs, const Gene& rhs) { return lhs.fitness < rhs.fitness; }

GA::GA(size_t width, size_t max_iterations, size_t max_population)
    : width_(width), max_iterations_(max_iterations), max_population_(max_population) {
    populations_.reserve(max_population_);
    childs_.reserve(max_population_);
}

std::vector<Schedule> GA::Solve(const Schedule& schedule, size_t expected_fit,
                                unsigned random_seed) {
    num_packets_ = schedule.getPackets().size();
    auto fit = Fitness(schedule);
    std::cout << "original schedule fit: " << fit << '\n';
    if (fit <= expected_fit) {
        std::cout << "original schedule has expected fit\n";
        return {schedule};
    }

    std::vector<RectWithPacket> rectangles_order = getRectanglesOrder(schedule);

    auto ordered_schedule = makeSchedule(rectangles_order);

    fit = Fitness(ordered_schedule);
    std::cout << "ordered schedule fit: " << fit << '\n';
    if (fit <= expected_fit) {
        std::cout << "ordered schedule has expected fit\n";
        return {ordered_schedule};
    }

    // Generate initial population.
    srand(random_seed);

    std::vector<Schedule> res;

    for (size_t i = 0; i < max_population_; i++) {
        Gene gene;
        gene.rectangles_order = SwapRectanglesOrder(rectangles_order);
        populations_.push_back(std::move(gene));
    }

    CalculateFitnesses();

    size_t iterations = 1;
    while (iterations < max_iterations_) {
        GenerateLikelihoods();
        CreateChilds();
        Mutation();
        CreateNewPopulation();
        auto min_fit = CalculateFitnesses();
        if (min_fit < static_cast<double>(expected_fit)) {
            std::cout << "iter: " << iterations << " found with fit: " << min_fit << '\n';
            break;
        }

        ++iterations;
    }

    std::sort(populations_.begin(), populations_.end());

    if (iterations == max_iterations_) {
        std::cout << "interrupted after " << iterations << " iterations\n";
    }

    for (size_t i = 0; i < populations_.size(); ++i) {
        res.push_back(makeSchedule(populations_[i].rectangles_order));
        std::cout << (i + 1) << " fit: " << populations_[i].fitness << '\n';
    }

    res.insert(res.begin(), ordered_schedule);

    return res;
}

void GA::PrintStatistic() {
    if (iterations_statistic.empty()) {
        std::cout << "satistic is emty\n";
        return;
    }

    // for (size_t i = 0; i < iterations_statistic.size(); ++i) {
    //     std::cout << "iter " << std::setw(5) << i << " min:" << iterations_statistic[i].min
    //               << " avg:" << iterations_statistic[i].avg << "\n";
    // }

    double scale = iterations_statistic[0].avg > 100.0 ? iterations_statistic[0].avg / 100 : 1.0;

    for (size_t i = 0; i < iterations_statistic.size(); ++i) {
        std::cout << std::setw(5) << i << std::string(iterations_statistic[i].min / scale, ' ')
                  << std::string(
                         (iterations_statistic[i].avg - iterations_statistic[i].min) / scale + 1,
                         '*')
                  << '\n';
    }
}

Schedule GA::makeSchedule(const std::vector<RectWithPacket>& rectangles_order) const {
    Schedule result_schedule(num_packets_);

    for (const auto rect_order : rectangles_order) {
        auto rect_with_pos = rect_order.rect_with_pos;

        rect_with_pos.pos.y = 0;
        for (size_t i = 0; i < rect_with_pos.rect->height; ++i) {
            rect_with_pos.pos.y =
                std::max(rect_with_pos.pos.y,
                         result_schedule.getPackets()[rect_with_pos.pos.x + i].max_width);
        }

        result_schedule.addRectangle(rect_order.packet->id, rect_with_pos);
    }

    return result_schedule;
}

std::vector<RectWithPacket> GA::getRectanglesOrder(const Schedule& schedule) const {
    std::vector<RectWithPacket> rectangles_order;

    for (const auto& packet : schedule.getPackets()) {
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
    const std::vector<RectWithPacket>& rectangles_order) const {
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
    // std::cout << "swap : " << *(result.begin() + first_rect_id)->rect_with_pos.rect << " and "
    //           << *(result.begin() + second_rect_id)->rect_with_pos.rect << '\n';
    std::iter_swap(result.begin() + first_rect_id, result.begin() + second_rect_id);

    return result;
}

double GA::CalculateFitnesses() {
    double avg_fit = 0;
    double min_fit = std::numeric_limits<double>::max();

    for (auto& gene : populations_) {
        auto schedule = makeSchedule(gene.rectangles_order);
        gene.fitness = Fitness(schedule);

        avg_fit += gene.fitness;
        min_fit = std::min(min_fit, gene.fitness);
    }

    assert(!populations_.empty());
    avg_fit /= static_cast<double>(populations_.size());

    iterations_statistic.push_back({min_fit, avg_fit});

    return min_fit;
}

double GA::Fitness(const Schedule& schedule) const {
    return static_cast<double>(schedule.OutOfRangeSize(width_));
}

void GA::GenerateLikelihoods() {
    double multinv = MultInv();

    double last = 0;
    const double one_hundred_percent = 100;
    for (auto& gene : populations_) {
        gene.likelihood = last = last + ((1 / (gene.fitness) / multinv) * one_hundred_percent);
        // std::cout << "% for " << i << " : " << last << '\n';
    }
}

double GA::MultInv() const {
    double sum = 0;

    for (const auto& gene : populations_) {
        sum += 1 / gene.fitness;
    }

    return sum;
}

void GA::CreateChilds() {
    childs_.clear();

    for (size_t i = 0; i < max_population_; i++) {
        size_t parent1 = 0;
        size_t parent2 = 0;
        size_t iteration = 0;
        const size_t max_iterations = 25;
        while (parent1 == parent2) {
            parent1 = GetIndex(static_cast<double>(rand() % 101));
            parent2 = GetIndex(static_cast<double>(rand() % 101));

            if (parent1 != parent2 && populations_[parent1] == populations_[parent2]) {
                // std::cout << "equal genes for " << parent1 << " and " << parent2 << '\n';
                parent1 = parent2;
                continue;
            }
            if (++iteration == max_iterations) {
                break;
            }
        }

        childs_.push_back(Crossover(parent1, parent2));
    }
}

size_t GA::GetIndex(double val) const {
    double last = 0;
    for (size_t i = 0; i < max_population_; i++) {
        if (last <= val && val <= populations_[i].likelihood) {
            return i;
        }

        last = populations_[i].likelihood;
    }

    return max_population_ - 1;
}

Gene GA::Crossover(size_t parent_id_1, size_t parent_id_2) const {
    // Which parent comes first?
    if (rand() % 2) {
        std::swap(parent_id_1, parent_id_2);
    }

    auto child = populations_[parent_id_1];  // Child is all first parent initially.
    child.likelihood = -1;

    size_t crossover =
        rand() % child.rectangles_order.size() + 1;  // Create the crossover point (not first).

    // std::cout << "crossover: " << p1 << " and " << p2 << " with " << crossover << '\n';

    auto rectangles_order = populations_[parent_id_2].rectangles_order;
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

    return child;
}

void GA::Mutation() {
    for (auto& gene : childs_) {
        if (rand() % 10 == 0) {
            gene.rectangles_order = SwapRectanglesOrder(gene.rectangles_order);
        }
    }
}

void GA::CreateNewPopulation() {
    populations_.insert(populations_.end(), childs_.begin(), childs_.end());
    std::sort(populations_.begin(), populations_.end());
    populations_.erase(std::unique(populations_.begin(), populations_.end()), populations_.end());
    populations_.erase(populations_.begin() + max_population_, populations_.end());
}