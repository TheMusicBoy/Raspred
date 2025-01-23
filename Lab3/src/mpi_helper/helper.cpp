#include <mpi_helper/helper.h>

namespace NMpi {

namespace {

////////////////////////////////////////////////////////////////////////////////

enum EMessageType {
    None = 0,
    Size = 1 << 20,
    Memory = 2 << 20
};

////////////////////////////////////////////////////////////////////////////////

} // namespace

////////////////////////////////////////////////////////////////////////////////

TMpiHelper::TMpiHelper(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &ThreadSize_);
    MPI_Comm_rank(MPI_COMM_WORLD, &CurrentThread_);
}

TMpiHelper::~TMpiHelper() {
    MPI_Finalize();
}

void TMpiHelper::Barrier() const {
    MPI_Barrier(MPI_COMM_WORLD);
}

void TMpiHelper::BcastMemory(void* data, size_t size) {
    MPI_Bcast(data, size, MPI_CHAR, CurrentThread_, MPI_COMM_WORLD);
}

void TMpiHelper::BcastSize(int number) {
    MPI_Bcast(&number, 1, MPI_INT, CurrentThread_, MPI_COMM_WORLD);
}

void TMpiHelper::BsendMemory(void* data, size_t size, int dst, int tag) {
    MPI_Bsend(data, size, MPI_CHAR, dst, tag | EMessageType::Memory, MPI_COMM_WORLD);
}

void TMpiHelper::BsendSize(int number, int dst, int tag) {
    MPI_Bsend(&number, 1, MPI_INT, dst, tag | EMessageType::Size, MPI_COMM_WORLD);
}

void TMpiHelper::SendMemory(void* data, size_t size, int dst, int tag) {
    MPI_Send(data, size, MPI_CHAR, dst, tag | EMessageType::Memory, MPI_COMM_WORLD);
}

void TMpiHelper::SendSize(int number, int dst, int tag) {
    MPI_Send(&number, 1, MPI_INT, dst, tag | EMessageType::Size, MPI_COMM_WORLD);
}

void TMpiHelper::RecieveMemory(void* output, size_t size, int src, int tag) {
    MPI_Status status;
    MPI_Recv(output, size, MPI_CHAR, src, tag | EMessageType::Memory, MPI_COMM_WORLD, &status);
}

int TMpiHelper::RecieveSize(int src, int tag) {
    int data;
    MPI_Status status;
    MPI_Recv(&data, 1, MPI_INT, src, tag | EMessageType::Size, MPI_COMM_WORLD, &status);
    return data;
}

void TMpiHelper::RecieveCastMemory(void* output, size_t size, int src) {
    MPI_Status status;
    MPI_Bcast(output, size, MPI_CHAR, src, MPI_COMM_WORLD);
}

int TMpiHelper::RecieveCastSize(int src) {
    int data;
    MPI_Status status;
    MPI_Bcast(&data, 1, MPI_INT, src, MPI_COMM_WORLD);
    return data;
}

int TMpiHelper::GetCurrentThread() const {
    return CurrentThread_;
}

int TMpiHelper::GetThreadCount() const {
    return ThreadSize_;
}

////////////////////////////////////////////////////////////////////////////////

} // namespace NMpi
