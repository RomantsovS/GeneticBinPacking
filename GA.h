#ifndef GA_H
#define GA_H

#include <cstdlib>
#include <vector>

#include "Schedule.h"

struct RectWithPacket {
    bool operator==(const RectWithPacket& other) const {
        return rect_with_pos == other.rect_with_pos && packet == other.packet;
    }

    RectWithPos rect_with_pos;
    const Packet* packet = nullptr;
};

class Gene {
   public:
    Gene() = default;

    std::vector<RectWithPacket> rectangles_order;
    size_t fitness{};
    double likelihood{-1};
};

bool operator==(const Gene& lhs, const Gene& rhs);
bool operator<(const Gene& lhs, const Gene& rhs);

class GA {
   public:
    GA(size_t width, size_t max_iterations, size_t max_pop);
    std::vector<Schedule> Solve(const Schedule& schedule, size_t expected_fit,
                                unsigned random_seed);

    void PrintStatistic();

   private:
    Schedule makeSchedule(const std::vector<RectWithPacket>& rectangles_order) const;
    std::vector<RectWithPacket> getRectanglesOrder(const Schedule& schedule) const;

    std::vector<RectWithPacket> SwapRectanglesOrder(
        const std::vector<RectWithPacket>& rectangles_order) const;

    size_t CalculateFitnesses();
    size_t Fitness(const Schedule& schedule) const;  // Fitness function.
    void CalculateLikelihoods();                     // Generate likelihoods.
    double MultInv() const;                          // Creates the multiplicative inverse.
    void CreateChilds();
    size_t GetIndex(double val) const;
    Gene Crossover(size_t parent_id_1, size_t parent_id_2) const;
    void Mutation();
    void CreateNewPopulation();

    size_t width_;
    size_t num_packets_;

    size_t max_iterations_;
    size_t max_population_;
    std::vector<Gene> population_;
    std::vector<Gene> childs_;

    struct iteration_statistic {
        size_t min;
        size_t avg;
        size_t size;
    };
    std::vector<iteration_statistic> iterations_statistic;
};

#endif