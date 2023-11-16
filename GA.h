#ifndef GA_H
#define GA_H

#include <cstdlib>
#include <vector>

#include "Schedule.h"

class Gene {
   public:
    Gene() = default;

    // Test for equality.
    bool operator==(Gene gn) {
        for (size_t i = 0; i < alleles.size(); i++) {
            if (gn.alleles[i] != alleles[i]) return false;
        }

        return true;
    }

    std::vector<size_t> alleles;
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
    double Fitness(Gene& gene);  // Fitness function.
    void GenerateLikelihoods();  // Generate likelihoods.
    double MultInv();            // Creates the multiplicative inverse.
    double CreateFitnesses();
    void CreateNewPopulation();
    int GetIndex(double val);
    Gene Crossover(int p1, int p2);

    size_t width_;
    const Schedule* schedule_;

    size_t max_iterations_;
    size_t max_population_;
    std::vector<Gene> populations_;
};

#endif