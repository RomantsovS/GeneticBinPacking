#include "GA.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>

GA::GA(int width, const std::vector<Rectangle>& rectangles, size_t max_iterations, size_t max_pop)
    : width_(width), rectangles_(&rectangles), max_iterations_(max_iterations), max_pop_(max_pop) {
    populations_.reserve(max_pop_);
}

int GA::Solve() {
    double fitness = -1;
    return fitness;

    // Generate initial population.
    // srand((unsigned)time(nullptr));

    // for (int i = 0; i < max_pop_; i++) {
    //     populations_.emplace_back();
    //     populations_.back().alleles.reserve(rectangles_->size());

    //     for (size_t i = 0; i < rectangles_->size(); ++i) {
    //         for (int j = 0; j < 100; ++j) {
    //             Rectangle r(rectangles_->at(i));
    //             r.pos.y = rand() % (width_ - r.width_);
    //             if (std::any_of(rectangles_->begin(), rectangles_->end(),
    //                             [&r](const Rectangle& other) { return r.intersect(other); })) {
    //                 continue;
    //             }
    //             populations_.back().alleles.emplace_back(r.pos);
    //             break;
    //         }
    //     }
    //     if (populations_.back().alleles.size() != rectangles_->size()) {
    //         throw std::runtime_error("can't place");
    //     }
    // }

    fitness = CreateFitnesses();

    int iterations = 0;  // Keep record of the iterations.
    while (fitness != 0 ||
           iterations < max_iterations_) {  // Repeat until solution found, or over 50 iterations.
        std::cout << "iter: " << iterations << '\n';
        GenerateLikelihoods();  // Create the likelihoods.
        CreateNewPopulation();
        fitness = CreateFitnesses();
        break;

        iterations++;
    }

    return -1;
}

double GA::Fitness(Gene& gn) { return 1.0; }

double GA::CreateFitnesses() {
    double avgfit = 0;
    double fitness = 0;
    for (int i = 0; i < max_pop_; i++) {
        fitness = Fitness(populations_[i]);
        avgfit += fitness;
        std::cout << "fit for " << i << " : " << fitness << '\n';
        if (fitness == 0) {
            return i;
        }
    }

    return 0;
}

double GA::MultInv() {
    double sum = 0;

    for (int i = 0; i < max_pop_; i++) {
        sum += 1 / (populations_[i].fitness);
    }

    return sum;
}

void GA::GenerateLikelihoods() {
    double multinv = MultInv();

    double last = 0;
    for (int i = 0; i < max_pop_; i++) {
        populations_[i].likelihood = last =
            last + ((1 / (populations_[i].fitness) / multinv) * 100);
        std::cout << "% for " << i << " : " << last << '\n';
    }
}

int GA::GetIndex(double val) {
    double last = 0;
    for (int i = 0; i < max_pop_; i++) {
        if (last <= val && val <= populations_[i].likelihood)
            return i;
        else
            last = populations_[i].likelihood;
    }

    return 4;
}

Gene GA::Crossover(int p1, int p2) {
    int crossover = rand() % 3 + 1;  // Create the crossover point (not first).
    int first = rand() % 100;        // Which parent comes first?

    auto child = populations_[p1];  // Child is all first parent initially.

    int initial = 0, final = 3;  // The crossover boundaries.
    if (first < 50)
        initial = crossover;  // If first parent first. start from crossover.
    else
        final = crossover + 1;  // Else end at crossover.

    for (int i = initial; i < final; i++) {  // Crossover!
        child.alleles[i] = populations_[p2].alleles[i];
        // if (rand() % 101 < 5) child.alleles[i] = rand() % (result + 1);
    }

    std::cout << "crossover: " << p1 << " and " << p2 << '\n';

    return child;  // Return the kid...
}

void GA::CreateNewPopulation() {
    std::vector<Gene> temppop(max_pop_);

    for (int i = 0; i < max_pop_; i++) {
        int parent1 = 0, parent2 = 0, iterations = 0;
        while (parent1 == parent2 || populations_[parent1] == populations_[parent2]) {
            parent1 = GetIndex((double)(rand() % 101));
            parent2 = GetIndex((double)(rand() % 101));
            if (++iterations > 25) break;
        }

        temppop[i] = Crossover(parent1, parent2);  // Create a child.
    }

    for (int i = 0; i < max_pop_; i++) populations_[i] = temppop[i];
}