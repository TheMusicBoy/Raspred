#include <iostream>
#include <mpi_helper/helper.h>

#include <algorithm>
#include <random>
#include <ctime>

int main(int argc, char** argv) {
    NMpi::TMpiHelper mpi(argc, argv);

    if (mpi.GetCurrentThread() == 0) {
        std::srand(std::time(nullptr));

        std::vector<int> mas(std::rand() % 100);
        std::generate(mas.begin(), mas.end(), []{ return std::rand() % 100; });

        for (const auto& e : mas) {
            std::cout << e << ' ';
        }
        std::cout << std::endl;

        mpi.Bcast(mas);
        int sum, max;
        mpi.RecieveReduceSum(&sum, 1);
        mpi.RecieveReduceMax(&max, 1);
        mpi.Barrier();
        std::cout << sum << ' ' << max << std::endl;
    } else {

        MPI_Op op;
        std::vector<int> result = mpi.RecieveCast<std::vector<int>>(0);
        int sum = std::accumulate(result.begin(), result.end(), 0) * mpi.GetCurrentThread();
        int max = *std::max_element(result.begin(), result.end()) * mpi.GetCurrentThread();
        mpi.SendReduceSum(&sum, 1, 0);
        mpi.SendReduceMax(&max, 1, 0);
        mpi.Barrier();
    }
}
