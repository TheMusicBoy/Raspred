#include <mpi/mpi.h>

namespace NMpi::NDetails {

////////////////////////////////////////////////////////////////////////////////

template <typename T>
struct TTypeTraits {
    static constexpr MPI_Datatype Type = 0;
    static constexpr bool IsComplex = true;
};

template<>
struct TTypeTraits<char> {
    static constexpr MPI_Datatype Type = MPI_CHAR;
    static constexpr bool IsComplex = false;
};

template<>
struct TTypeTraits<unsigned char> {
    static constexpr MPI_Datatype Type = MPI_UNSIGNED_CHAR;
    static constexpr bool IsComplex = false;
};

template<>
struct TTypeTraits<int8_t> {
    static constexpr MPI_Datatype Type = MPI_INT8_T;
    static constexpr bool IsComplex = false;
};

template<>
struct TTypeTraits<short> {
    static constexpr MPI_Datatype Type = MPI_INT16_T;
    static constexpr bool IsComplex = false;
};

template<>
struct TTypeTraits<unsigned short> {
    static constexpr MPI_Datatype Type = MPI_UINT16_T;
    static constexpr bool IsComplex = false;
};

template<>
struct TTypeTraits<int32_t> {
    static constexpr MPI_Datatype Type = MPI_INT32_T;
    static constexpr bool IsComplex = false;
};

template<>
struct TTypeTraits<uint32_t> {
    static constexpr MPI_Datatype Type = MPI_UINT32_T;
    static constexpr bool IsComplex = false;
};

template<>
struct TTypeTraits<long> {
    static constexpr MPI_Datatype Type = MPI_LONG;
    static constexpr bool IsComplex = false;
};

template<>
struct TTypeTraits<unsigned long> {
    static constexpr MPI_Datatype Type = MPI_UNSIGNED_LONG;
    static constexpr bool IsComplex = false;
};

template<>
struct TTypeTraits<long long> {
    static constexpr MPI_Datatype Type = MPI_LONG_LONG;
    static constexpr bool IsComplex = false;
};

template<>
struct TTypeTraits<unsigned long long> {
    static constexpr MPI_Datatype Type = MPI_UNSIGNED_LONG_LONG;
    static constexpr bool IsComplex = false;
};

template<>
struct TTypeTraits<float> {
    static constexpr MPI_Datatype Type = MPI_FLOAT;
    static constexpr bool IsComplex = false;
};

template<>
struct TTypeTraits<double> {
    static constexpr MPI_Datatype Type = MPI_DOUBLE;
    static constexpr bool IsComplex = false;
};

template<>
struct TTypeTraits<long double> {
    static constexpr MPI_Datatype Type = MPI_LONG_DOUBLE;
    static constexpr bool IsComplex = false;
};

////////////////////////////////////////////////////////////////////////////////

} // namespace NMpi::NDetails
