#include <mpi/mpi.h>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <numeric>
#include <random>
#include <ctime>
#include <iomanip>

template <typename Type>
std::ostream& operator<<(std::ostream& os, const std::vector<Type>& value) {
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

    std::mt19937 mt(std::time(nullptr));
    constexpr size_t size = 1000;
    std::vector<std::vector<int>> A(size, std::vector<int>(size));
    std::vector<std::vector<int>> B(size, std::vector<int>(size));
    for (auto& l : A) {
        for (auto& e : l) {
            e = mt() % 10;
        }
    }

    for (auto& l : B) {
        for (auto& e : l) {
            e = mt() % 10;
        }
    }

    std::vector<std::vector<int>> C(size, std::vector<int>(size, 0));

    if (thread == 0) {
        auto start = MPI_Wtime();
        for (int i = 0; i < size; i++) {
            for (int r = 0; r < size; r++) {
                for (int ii = 0; ii < size; ii++) {
                    C[i][r] += A[i][ii] * B[r][ii];
                }
            }
        }
        std::cout << "Sync time: " << MPI_Wtime() - start << std::endl;
    }

    const size_t chunkSize = size / tCount;
    std::vector<int> bufferA(chunkSize);
    std::vector<int> bufferB(chunkSize);

    auto start = MPI_Wtime();
    for (int i = 0; i < size; i++) {
        for (int r = 0; r < size; r++) {
            int sum = 0;
            MPI_Scatter(A[i].data(), chunkSize, MPI_INT, bufferA.data(), chunkSize, MPI_INT, 0, MPI_COMM_WORLD);
            MPI_Scatter(B[r].data(), chunkSize, MPI_INT, bufferB.data(), chunkSize, MPI_INT, 0, MPI_COMM_WORLD);
            for (int ii = 0; ii < chunkSize; ii++) {
                sum += bufferA[ii] * bufferB[ii];
            }

            int gsum = 0;
            MPI_Reduce(&sum, &gsum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
            if (thread == 0) {
                C[i][r] = gsum;
            }
        }
    }

    if (thread == 0) {
        std::cout << "Async time: " << MPI_Wtime() - start << std::endl;
    }

    MPI_Finalize();
}
