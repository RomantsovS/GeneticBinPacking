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

    bool operator==(const Gene& other) { return rectangles_order == other.rectangles_order; }

    std::vector<RectWithPacket> rectangles_order;
    double fitness{-1};
    double likelihood{-1};
};

class GA {
   public:
    GA(size_t width, const Schedule& schedule, size_t max_iterations, size_t max_pop);
    Schedule Solve();

    // Returns a given gene.
    Gene GetGene(size_t i) { return populations_[i]; }

   private:
    Schedule makeSchedule(const std::vector<RectWithPacket>& rectangles_order);
    std::vector<RectWithPacket> getRectanglesOrder(const Schedule& schedule);

    std::vector<RectWithPacket> SwapRectanglesOrder(
        const Schedule& schedule, const std::vector<RectWithPacket>& rectangles_order);

    double Fitness(const Gene& gene);  // Fitness function.
    void GenerateLikelihoods();        // Generate likelihoods.
    double MultInv();                  // Creates the multiplicative inverse.
    void CreateChilds();
    int GetIndex(double val);
    Gene Crossover(int p1, int p2);

    size_t width_;
    const Schedule* schedule_;

    size_t max_iterations_;
    size_t max_population_;
    std::vector<Gene> populations_;
};

#endif