#include <mpi/mpi.h>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>

int main(int argc, char** argv) {
    int tCount, thread;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &tCount);
    MPI_Comm_rank(MPI_COMM_WORLD, &thread);

    if (thread == 0) {
        for (int i = 1; i < tCount; i++) {
            int message = i * 2;
            MPI_Send(&message, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                    // value, count, type, dst, tag
            std::cout << "Process " << thread << " sent message " << message << " to " << i << std::endl;
        }
    } else {
        int value;
        MPI_Recv(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        std::cout << "Process " << thread << " got message " << value << " from 0" << std::endl;
    }
    
    MPI_Finalize();
}
