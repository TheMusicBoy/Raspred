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
    }

    auto start = MPI_Wtime();
    std::vector<int> chunk(4);
    MPI_Scatter(mas.data(), chunk.size(), MPI_INT, chunk.data(), chunk.size(), MPI_INT, 0, MPI_COMM_WORLD);

    for (auto& e : chunk) {
        e *= thread;
    }

    MPI_Gather(chunk.data(), chunk.size(), MPI_INT, mas.data(), chunk.size(), MPI_INT, 0, MPI_COMM_WORLD);
    
    if (thread == 0) {
        std::cout << "Sync " << MPI_Wtime() - start << 's' << std::endl;
        std::cout << "Result array: " << mas << std::endl;
    }
    start = MPI_Wtime();

    MPI_Request request;
    MPI_Iscatter(mas.data(), chunk.size(), MPI_INT, chunk.data(), chunk.size(), MPI_INT, 0, MPI_COMM_WORLD, &request);

    for (auto& e : chunk) {
        e *= thread;
    }

    MPI_Igather(chunk.data(), chunk.size(), MPI_INT, mas.data(), chunk.size(), MPI_INT, 0, MPI_COMM_WORLD, &request);

    MPI_Wait(&request, MPI_STATUS_IGNORE);
    
    if (thread == 0) {
        std::cout << "Async " << MPI_Wtime() - start << 's' << std::endl;
        std::cout << "Result array: " << mas << std::endl;
    }

    MPI_Finalize();
}
