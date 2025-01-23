#include <mpi/mpi.h>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <numeric>
#include <sstream>

int main(int argc, char** argv) {
    int tCount, thread;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &tCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &thread);

    std::vector<int> mas(10);
    if (thread == 0) {
        std::iota(mas.begin(), mas.end(), 1);
    }

    MPI_Bcast(mas.data(), mas.size(), MPI_INT, 0, MPI_COMM_WORLD);

    int sum = std::accumulate(mas.begin(), mas.end(), 0) * thread;
    int max = *std::max_element(mas.begin(), mas.end()) * thread;

    int rsum, rmax;
    MPI_Reduce(&sum, &rsum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    MPI_Reduce(&max, &rmax, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    if (thread == 0) {
        std::cout << "Sum: " << rsum << ", Max: " << rmax << std::endl;
    }
    
    MPI_Finalize();
}
