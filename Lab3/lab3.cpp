#include <iostream>
#include <mpi_helper/helper.h>

#include <algorithm>
#include <random>
#include <ctime>

int main(int argc, char** argv) {
    NMpi::TMpiHelper mpi(argc, argv);

    if (mpi.GetCurrentThread() == 0) {
        std::srand(std::time(nullptr));

        std::vector<int> mas(4 * mpi.GetThreadCount());
        std::iota(mas.begin(), mas.end(), 1);

        std::cout << NFormat::Format("Start - {}", mas) << std::endl;

        std::vector<int> result = mpi.SendScatter(mas);
        for (auto& e : result) {
            e *= mpi.GetCurrentThread();
        }

        auto subResult = mpi.RecieveGather<int>(result);
        std::cout << NFormat::Format("All elements - {}", subResult) << std::endl;

        int sum = mpi.AllReduceSum(std::accumulate(result.begin(), result.end(), 0));
        std::cout << NFormat::Format("Thread {} have sum of elements = {}", mpi.GetCurrentThread(), sum) << std::endl;

        mpi.Barrier();
    } else {

        MPI_Op op;
        std::vector<int> result = mpi.RecieveScatter<int>(0);
        for (auto& e : result) {
            e *= mpi.GetCurrentThread();
        }

        mpi.SendGather(result, 0);
        
        int sum = mpi.AllReduceSum(std::accumulate(result.begin(), result.end(), 0));
        std::cout << NFormat::Format("Thread {} have sum of elements = {}", mpi.GetCurrentThread(), sum) << std::endl;

        mpi.Barrier();
    }
}
