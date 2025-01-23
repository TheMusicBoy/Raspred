#include <mpi/mpi.h>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <numeric>
#include <sstream>


std::ostream& operator<<(std::ostream& os, const std::vector<int>& value) {
    for (auto iter_ = value.begin(); iter_ != value.end(); iter_++) {
        os << *iter_ << ' ';
    }
    return os;
}

int main(int argc, char** argv) {
    int tCount, thread;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &tCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &thread);

    std::vector<int> mas(4 * tCount);
    if (thread == 0) {
        std::iota(mas.begin(), mas.end(), 1);
        std::cout << "Process 0 scattered mas: " << mas << std::endl;
    }

    std::vector<int> chunk(4);
    MPI_Scatter(mas.data(), chunk.size(), MPI_INT, chunk.data(), chunk.size(), MPI_INT, 0, MPI_COMM_WORLD);

    for (auto& e : chunk) {
        e *= thread;
    }

    MPI_Gather(chunk.data(), chunk.size(), MPI_INT, mas.data(), chunk.size(), MPI_INT, 0, MPI_COMM_WORLD);
    
    if (thread == 0) {
        std::cout << "Process 0 gathered mas: " << mas << std::endl;
    }


    int sum = std::accumulate(chunk.begin(), chunk.end(), 0);
    int gsum;
    MPI_Allreduce(&sum, &gsum, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    std::cout << "Process " << thread << " got local sum: " << sum << "; and total sum: " << gsum << std::endl;
    
    MPI_Finalize();
}
