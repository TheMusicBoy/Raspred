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
    long long tests = 1000000000;
    long long success = 0;
    for (int i = 0; i < tests; i++) {
        constexpr long long center = UINT32_MAX >> 1;
        long long x = 1ull - center + mt();
        long long y = 1ull - center + mt();

        success += x * x + y * y < center * center;
    }

    long long result;
    MPI_Reduce(&success, &result, 1, MPI_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (thread == 0) {
        double pi = 4.0 * double(result) / double(tests * tCount);
        std::cout << "Pi is: " << std::fixed << std::setprecision(10) << pi << std::endl;
    }

    MPI_Finalize();
}
