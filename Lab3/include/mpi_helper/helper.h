#include <mpi/mpi.h>

#include <mpi_helper/details.h>
#include <common/exception.h>
#include <common/format.h>
#include <encoder/encoder.h>

#include <numeric>
#include <string>
#include <cstring>
#include <iostream>
#include <memory>

namespace NMpi {

////////////////////////////////////////////////////////////////////////////////

class TMpiHelper {
public:
    TMpiHelper(int argc, char** argv);
    ~TMpiHelper();

    void Barrier() const;

    template <typename Type>
    struct TFutureBody {
        const Type& get() {
            if (Inited_) {
                MPI_Status status;
                MPI_Wait(&Request_, &status);
                ::DecodeArray(Value_.data(), Value_.size(), RawData_.data());
                Inited_ = true;
            }
            return Value_;
        }

        void Clear() {
            Buffer_.clear();
        }

        Type Value_;
        std::string RawData_;
        std::string Buffer_;
        MPI_Request Request_;
        bool Inited_ = false;
    };

    template <typename Type>
    using TFuturePtr = std::shared_ptr<TFutureBody<Type>>;

    template <typename Type>
    requires (NDetails::TTypeTraits<Type>::IsComplex)
    void Bsend(const Type& value, int dst, int tag = 0) {
        std::string data(::TMessageEncoder<Type>::Size(value), ' ');
        ::TMessageEncoder<Type>::Encode(value, data.data());
        BsendSize(data.size(), dst, tag);
        BsendMemory(data.data(), data.size(), dst, tag);
    }

    template <typename Type>
    void Bsend(Type* value, int dst, int tag = 0) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        MPI_Bsend(value, 1, NDetails::TTypeTraits<Type>::Type, dst, tag, MPI_COMM_WORLD);
    }

    template <typename Type>
    void Bsend(Type* value, int size, int dst, int tag = 0) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        MPI_Bsend(value, size, NDetails::TTypeTraits<Type>::Type, dst, tag, MPI_COMM_WORLD);
    }

    ////////////////////////////////////////////////////////////////////////////////

    template <typename Type>
    requires (NDetails::TTypeTraits<Type>::IsComplex)
    void Send(const Type& value, int dst, int tag = 0) {
        std::string data(::TMessageEncoder<Type>::Size(value), ' ');
        ::TMessageEncoder<Type>::Encode(value, data.data());
        SendSize(data.size(), dst, tag);
        SendMemory(data.data(), data.size(), dst, tag);
    }

    template <typename Type>
    void Send(Type* value, int dst, int tag = 0) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        MPI_Send(value, 1, NDetails::TTypeTraits<Type>::Type, dst, tag, MPI_COMM_WORLD);
    }

    template <typename Type>
    void Send(Type* value, int size, int dst, int tag = 0) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        MPI_Send(value, size, NDetails::TTypeTraits<Type>::Type, dst, tag, MPI_COMM_WORLD);
    }

    ////////////////////////////////////////////////////////////////////////////////

    template <typename Type>
    requires (NDetails::TTypeTraits<Type>::IsComplex)
    void Bcast(const Type& value) {
        std::string data(::TMessageEncoder<Type>::Size(value), ' ');
        ::TMessageEncoder<Type>::Encode(value, data.data());
        BcastMemory(data.data(), data.size());
    }

    template <typename Type>
    requires (!NDetails::TTypeTraits<Type>::IsComplex)
    void Bcast(const Type& value) {
        std::string data(::TMessageEncoder<Type>::Size(value), ' ');
        ::TMessageEncoder<Type>::Encode(value, data.data());
        BcastMemory(data.data(), data.size());
    }

    template <typename Type>
    void Bcast(Type* value, int dst, int tag = 0) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        MPI_Bcast(value, 1, NDetails::TTypeTraits<Type>::Type, CurrentThread_, MPI_COMM_WORLD);
    }

    template <typename Type>
    void Bcast(Type* value, int size, int dst, int tag = 0) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        MPI_Bcast(value, size, NDetails::TTypeTraits<Type>::Type, CurrentThread_, MPI_COMM_WORLD);
    }
    
    ////////////////////////////////////////////////////////////////////////////////
    
    template <typename Type>
    std::vector<Type> Scatter(const std::vector<Type>& data) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        THROW_EXCEPTION_IF(data.size() % ThreadSize_, "Data size must be devided py thread count");
        static size_t eSize = TMessageEncoder<Type>::Size(Type());

        size_t eCount = data.size() / ThreadSize_;
        std::vector<char> recvData(eCount * eSize);

        Bcast(eCount);

        std::vector<char> sendData(data.size() * eSize);
        ::EncodeToArray(data.data(), data.size(), sendData.data());

        MPI_Scatter(
            // Send data
            sendData.data(),
            recvData.size(),
            MPI_CHAR,
            // Recieve data
            recvData.data(),
            recvData.size(),
            MPI_CHAR,
            // Meta
            CurrentThread_,
            MPI_COMM_WORLD);

        std::vector<Type> result(data.size() / ThreadSize_);
        ::DecodeArray(result.data(), result.size(), recvData.data());
        return result;
    }

    template <typename Type>
    std::vector<Type> SendScatter(const std::vector<Type>& data) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        THROW_EXCEPTION_IF(data.size() % ThreadSize_, "Data size must be devided py thread count");
        static size_t eSize = TMessageEncoder<Type>::Size(Type());

        size_t eCount = data.size() / ThreadSize_;
        std::vector<char> recvData(eCount * eSize);

        Bcast(eCount);

        std::vector<char> sendData(data.size() * eSize);
        ::EncodeToArray(data.data(), data.size(), sendData.data());

        MPI_Scatter(
            // Send data
            sendData.data(),
            recvData.size(),
            MPI_CHAR,
            // Recieve data
            recvData.data(),
            recvData.size(),
            MPI_CHAR,
            // Meta
            CurrentThread_,
            MPI_COMM_WORLD);

        std::vector<Type> result(data.size() / ThreadSize_);
        ::DecodeArray(result.data(), result.size(), recvData.data());
        return result;
    }

    template <typename Type>
    TFuturePtr<std::vector<Type>> ISendScatter(const std::vector<Type>& data, std::string& buffer) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        THROW_EXCEPTION_IF(data.size() % ThreadSize_, "Data size must be devided py thread count");
        static size_t eSize = TMessageEncoder<Type>::Size(Type());

        size_t eCount = data.size() / ThreadSize_;


        auto future = std::make_shared<TFutureBody<std::vector<Type>>>();
        future->Value_ = std::vector<Type>(eCount);
        future->RawData_ = std::string(eCount * eSize, ' ');
        buffer = std::string(data.size() * eSize, 'A');

        ::EncodeToArray(data.data(), data.size(), buffer.data());

        MPI_Iscatter(
            // Send data
            buffer.data(),
            future->RawData_.size(),
            MPI_CHAR,
            // Recieve data
            future->RawData_.data(),
            future->RawData_.size(),
            MPI_CHAR,
            // Meta
            CurrentThread_,
            MPI_COMM_WORLD,
            &future->Request_);

        return future;
    }

    template <typename Type>
    std::vector<Type> RecieveScatter(int src) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        static size_t eSize = TMessageEncoder<Type>::Size(Type());

        size_t eCount = RecieveCast<size_t>(src);
        std::vector<char> recvData(eCount * eSize);

        MPI_Scatter(
            // Send data
            nullptr,
            recvData.size(),
            MPI_CHAR,
            // Recieve data
            recvData.data(),
            recvData.size(),
            MPI_CHAR,
            // Meta
            src,
            MPI_COMM_WORLD);

        std::vector<Type> result(eCount);
        ::DecodeArray(result.data(), result.size(), recvData.data());
        return result;
    }

    template <typename Type>
    TFuturePtr<std::vector<Type>> IRecieveScatter(int src, size_t eCount) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        static size_t eSize = TMessageEncoder<Type>::Size(Type());

        auto future = std::make_shared<TFutureBody<std::vector<int>>>();
        future->Value_ = std::vector<Type>(eCount);
        future->RawData_ = std::string(eCount * eSize, ' ');

        MPI_Iscatter(
            // Send data
            nullptr,
            future->RawData_.size(),
            MPI_CHAR,
            // Recieve data
            future->RawData_.data(),
            future->RawData_.size(),
            MPI_CHAR,
            // Meta
            src,
            MPI_COMM_WORLD,
            &future->Request_);

        return future;
    }

    ////////////////////////////////////////////////////////////////////////////////

    template <typename Type>
    void SendGather(const std::vector<Type>& data, int dst) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        static size_t eSize = TMessageEncoder<Type>::Size(Type());

        size_t eCount = RecieveCast<size_t>(dst);
        THROW_EXCEPTION_IF(eCount != data.size(), "Thread {} sent data with size {}, expected {}", CurrentThread_, data.size(), eCount);

        std::vector<char> sendData(data.size() * eSize);
        ::EncodeToArray(data.data(), data.size(), sendData.data());

        MPI_Gather(
            // Send data
            sendData.data(),
            sendData.size(),
            MPI_CHAR,
            // Recieve data
            nullptr,
            sendData.size(),
            MPI_CHAR,
            // Meta
            dst,
            MPI_COMM_WORLD);
    }

    template <typename Type>
    void ISendGather(const std::vector<Type>& data, int dst) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        static size_t eSize = TMessageEncoder<Type>::Size(Type());

        std::vector<char> sendData(data.size() * eSize);
        ::EncodeToArray(data.data(), data.size(), sendData.data());

        MPI_Gather(
            // Send data
            sendData.data(),
            sendData.size(),
            MPI_CHAR,
            // Recieve data
            nullptr,
            sendData.size(),
            MPI_CHAR,
            // Meta
            dst,
            MPI_COMM_WORLD);
    }

    template <typename Type>
    std::vector<Type> RecieveGather(const std::vector<Type>& data) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        static size_t eSize = TMessageEncoder<Type>::Size(Type());
        
        Bcast(data.size());
        std::vector<char> recvData(data.size() * eSize * ThreadSize_);

        std::vector<char> sendData(data.size() * eSize);
        ::EncodeToArray(data.data(), data.size(), sendData.data());

        MPI_Gather(
            // Send data
            sendData.data(),
            sendData.size(),
            MPI_CHAR,
            // Recieve data
            recvData.data(),
            sendData.size(),
            MPI_CHAR,
            // Meta
            CurrentThread_,
            MPI_COMM_WORLD);

        std::vector<Type> result(data.size() * ThreadSize_);
        ::DecodeArray(result.data(), result.size(), recvData.data());
        return result;
    }

    template <typename Type>
    TFuturePtr<std::vector<Type>> IRecieveGather(const std::vector<Type>& data) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        static size_t eSize = TMessageEncoder<Type>::Size(Type());
        
        std::vector<char> sendData(data.size() * eSize);
        ::EncodeToArray(data.data(), data.size(), sendData.data());

        auto future = std::make_shared<TFutureBody<std::vector<int>>>();
        future->Value_ = std::vector<Type>(data.size() * ThreadSize_);
        future->RawData_ = std::string(data.size() * eSize * ThreadSize_, ' ');

        MPI_Igather(
            // Send data
            sendData.data(),
            sendData.size(),
            MPI_CHAR,
            // Recieve data
            future->RawData_.data(),
            sendData.size(),
            MPI_CHAR,
            // Meta
            CurrentThread_,
            MPI_COMM_WORLD,
            &future->Request_);

        return future;
    }

    ////////////////////////////////////////////////////////////////////////////////
    
    
    template <typename Type>
    requires (NDetails::TTypeTraits<Type>::IsComplex)
    Type RecieveCast(int src) {
        size_t size = RecieveCastSize(src);
        std::string data(size, ' ');
        RecieveCastMemory(data.data(), data.size(), src);
        
        Type value;
        ::TMessageEncoder<Type>::Decode(value, data.data());
        return value;
    }

    template <typename Type>
    requires (!NDetails::TTypeTraits<Type>::IsComplex)
    Type RecieveCast(int src) {
        static const size_t eSize = TMessageEncoder<Type>::Size(Type());
        std::string data(eSize, ' ');
        RecieveCastMemory(data.data(), data.size(), src);
        
        Type value;
        ::TMessageEncoder<Type>::Decode(value, data.data());
        return value;
    }
    
    template <typename Type>
    void RecieveCast(Type* output, int size, int src) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        MPI_Bcast(output, size, NDetails::TTypeTraits<Type>::Type, src, MPI_COMM_WORLD);
    }

    ////////////////////////////////////////////////////////////////////////////////
    
    template <typename Type>
    requires (NDetails::TTypeTraits<Type>::IsComplex)
    Type Recieve(int src, int tag = 0) {
        size_t size = RecieveSize(src, tag);
        std::string data(size, ' ');
        RecieveMemory(data.data(), data.size(), src, tag);
        
        Type value;
        ::TMessageEncoder<Type>::Decode(value, data.data());
        return value;
    }

    template <typename Type>
    requires (!NDetails::TTypeTraits<Type>::IsComplex)
    Type Recieve(int src, int tag = 0) {
        Type value;
        MPI_Recv(&value, 1, NDetails::TTypeTraits<Type>::Type, src, tag, MPI_COMM_WORLD);
        return value;
    }

    template <typename Type>
    void Recieve(Type* output, int size, int src, int tag = 0) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        MPI_Recv(output, size, NDetails::TTypeTraits<Type>::Type, src, tag, MPI_COMM_WORLD);
    }
    
    ////////////////////////////////////////////////////////////////////////////////

#define DEFINE_REDUCE_OPERATION(Postfix, Op) \
    template <typename Type> \
    void SendReduce##Postfix(const Type& data, int dst) { \
        SendReduceBase(&data, 1, Op, dst); \
    } \
\
    template <typename Type> \
    void SendReduce##Postfix(const std::vector<Type>& data, int dst) { \
        SendReduceBase(data.data(), data.size(), Op, dst); \
    } \
\
    template <typename Type> \
    Type RecieveReduce##Postfix(Type data) { \
        Type result; \
        RecieveReduceBase(&data, 1, Op, &result); \
        return result; \
    } \
 \
    template <typename Type> \
    std::vector<Type> RecieveReduce##Postfix(const std::vector<Type>& data) { \
        std::vector<Type> result; \
        RecieveReduceBase(data.data(), result.size(), Op, result.data()); \
        return result; \
    } \
 \
    template <typename Type> \
    Type AllReduce##Postfix(const Type& data) { \
        Type result; \
        AllReduceBase(&data, 1, Op, &result); \
        return result; \
    } \
 \
    template <typename Type> \
    std::vector<Type> AllReduce##Postfix(const std::vector<Type>& data) { \
        std::vector<Type> result; \
        AllReduceBase(data.data(), data.size(), Op, result.data()); \
        return result; \
    } \
    
    DEFINE_REDUCE_OPERATION(Min, MPI_MIN);
    DEFINE_REDUCE_OPERATION(Max, MPI_MAX);
    DEFINE_REDUCE_OPERATION(Minloc, MPI_MINLOC);
    DEFINE_REDUCE_OPERATION(Maxloc, MPI_MAXLOC);
    DEFINE_REDUCE_OPERATION(Sum, MPI_SUM);
    DEFINE_REDUCE_OPERATION(Prod, MPI_PROD);
    DEFINE_REDUCE_OPERATION(Land, MPI_LAND);
    DEFINE_REDUCE_OPERATION(Lor, MPI_LOR);
    DEFINE_REDUCE_OPERATION(Lxor, MPI_LXOR);
    DEFINE_REDUCE_OPERATION(Band, MPI_BAND);
    DEFINE_REDUCE_OPERATION(Bor, MPI_BOR);
    DEFINE_REDUCE_OPERATION(Bxor, MPI_BXOR);

    int GetCurrentThread() const;

    int GetThreadCount() const;

private:
    template <typename Type>
    void AllReduceBase(const Type* input, size_t size, MPI_Op op, Type* output) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        MPI_Allreduce(input, output, size, NDetails::TTypeTraits<Type>::Type, op, MPI_COMM_WORLD);
    }

    template <typename Type>
    void SendReduceBase(Type* data, size_t size, MPI_Op op, int dst) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        MPI_Reduce(data, nullptr, size, NDetails::TTypeTraits<Type>::Type, op, dst, MPI_COMM_WORLD);
    }

    template <typename Type>
    void RecieveReduceBase(const Type* input, size_t size, MPI_Op op, Type* output) {
        static_assert(!NDetails::TTypeTraits<Type>::IsComplex, "Value to send must be non complex");
        MPI_Reduce(input, output, size, NDetails::TTypeTraits<Type>::Type, op, CurrentThread_, MPI_COMM_WORLD);
    }

    void BcastMemory(void* data, size_t size);
    void BcastSize(int number);

    void SendMemory(void* data, size_t size, int dst, int tag = 0);
    void SendSize(int number, int dst, int tag = 0);

    void BsendMemory(void* data, size_t size, int dst, int tag = 0);
    void BsendSize(int number, int dst, int tag = 0);

    void RecieveMemory(void* output, size_t size, int src, int tag = 0);
    int RecieveSize(int src, int tag = 0);

    void RecieveCastMemory(void* output, size_t size, int src);
    int RecieveCastSize(int src);

    int ThreadSize_;
    int CurrentThread_;

    std::unordered_map<void*, MPI_Op> RegisteredOperators_;
    std::unordered_map<size_t, MPI_Datatype> RegisteredDatatypes_;
};

////////////////////////////////////////////////////////////////////////////////

} // namespace NMpi
