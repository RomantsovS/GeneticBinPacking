#ifndef GA_D_H
#define GA_D_H

#define MAXPOP 25

struct gene {
    int alleles[4];
    int fitness;
    float likelihood;

    // Test for equality.
    bool operator==(gene gn) {
        for (int i = 0; i < 4; i++) {
            if (gn.alleles[i] != alleles[i]) return false;
        }

        return true;
    }
};

class CDiophantine {
   public:
    CDiophantine(int, int, int, int, int);  // Constructor with coefficients for a,b,c,d.
    int Solve();                            // Solve the equation.

    // Returns a given gene.
    gene GetGene(int i) { return population[i]; }

   protected:
    int ca, cb, cc, cd;  // The coefficients.
    int result;
    gene population[MAXPOP];  // Population.

    int Fitness(gene &);         // Fitness function.
    void GenerateLikelihoods();  // Generate likelihoods.
    float MultInv();             // Creates the multiplicative inverse.
    int CreateFitnesses();
    void CreateNewPopulation();
    int GetIndex(float val);

    gene Breed(int p1, int p2);
};

#endif