#include <iostream>
#include <mpi_helper/helper.h>

#include <algorithm>
#include <random>
#include <chrono>

int main(int argc, char** argv) {
    NMpi::TMpiHelper mpi(argc, argv);

    std::vector<int> mainData(4 * mpi.GetCurrentThread());

    auto startTime = MPI_Wtime();
    
    if (mpi.GetCurrentThread() == 0) {
        std::iota(mainData.begin(), mainData.end(), 1);
    }

    


    for (auto& e : firstChunk) {
        e *= mpi.GetCurrentThread();
    }

        std::vector<int> firstFull = mpi.RecieveGather(firstChunk);
        
        auto duration = std::chrono::system_clock::now() - start;
        std::cout << NFormat::Format("First sync {} mcs - {}", duration.count(), firstFull) << std::endl;;
        mpi.Barrier();

        start = std::chrono::system_clock::now();
        {
            size_t chunkSize = firstFull.size() / mpi.GetThreadCount();

            std::vector<int> secondChunk(chunkSize);

            MPI_Iscatter(firstFull.data(), firstFull.size, MPI_INT, localArray.data(), local_array_size, MPI_INT, 0, MPI_COMM_WORLD, &request);
            MPI_Status status;j
            MPI_Wait(&request, &status);
            
            for (auto& e : secondChunk) {
                std::cout << e << std::endl;
                e *= mpi.GetCurrentThread();
            }

            std::cout << NFormat::Format("Thread {}, chunk = {}", mpi.GetCurrentThread(), secondChunk) << std::endl;
            std::vector<int> secondFull = mpi.IRecieveGather(secondChunk)->get();
            
            duration = std::chrono::system_clock::now() - start;
            std::cout << NFormat::Format("Second async {} mcs - {}", duration.count(), secondFull) << std::endl;
        }


    } else {
        std::vector<int> firstChunk = mpi.RecieveScatter<int>(0);
        
        for (auto& e : firstChunk) {
            e *= mpi.GetCurrentThread();
        }

        mpi.SendGather(firstChunk, 0);
        mpi.Barrier();
        
        std::vector<int> secondChunk = mpi.IRecieveScatter<int>(0, 4)->get();

        for (auto& e : secondChunk) {
            e *= mpi.GetCurrentThread();
        }

        std::cout << NFormat::Format("Thread {}, chunk = {}", mpi.GetCurrentThread(), secondChunk) << std::endl;
        mpi.ISendGather(secondChunk, 0);
    }
}
