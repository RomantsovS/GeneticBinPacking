#include <algorithm>
#include <iostream>

#include "GA_d.h"

int main() {
    CDiophantine dp(1, 2, 3, 4, 30);

    int ans;
    ans = dp.Solve();
    if (ans == -1) {
        std::cout << "No solution found." << std::endl;
    } else {
        gene gn = dp.GetGene(ans);

        std::cout << "The solution set to a+2b+3c+4d=30 is:\n";
        std::cout << "a = " << gn.alleles[0] << "." << std::endl;
        std::cout << "b = " << gn.alleles[1] << "." << std::endl;
        std::cout << "c = " << gn.alleles[2] << "." << std::endl;
        std::cout << "d = " << gn.alleles[3] << "." << std::endl;
    }

    return 0;
}
