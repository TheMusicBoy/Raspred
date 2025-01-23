#include <iostream>
// It can encode and decode:
//  - vector;
//  - deque;
//  - list;
//  - map;
//  - set;
//  - pair;
//  - tuple;
//  - string;
//  - Any static objects;
// 
//  Recursion is allowed!
//
//  You can add your own message encoder:
//
//  template<>
//  class TMessageEncoder<YourType> {
//      static size_t Size(const YourType& data);
//
//      static void* Decode(YourType& data, void* input);
//
//      static void* Encode(const YourType& data, void* output);
//
//  };
//

#include <common/exception.h>

#include <cstring>

#include <vector>
#include <list>
#include <deque>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <tuple>
#include <string>

////////////////////////////////////////////////////////////////////////////////

// Base type encoding
template <typename Type>
struct TMessageEncoder {
    static_assert(!std::is_pointer_v<Type>, "Pointer is not allowed to send");

    static size_t Size(const Type& data) {
        return alignof(Type);
    }

    static void* Decode(Type& value, void* mem) {
        value = *reinterpret_cast<Type*>(mem);
        return reinterpret_cast<Type*>(mem) + 1;
    }

    static void* Encode(const Type& data, void* output) {
        std::memcpy(output, &data, alignof(Type));
        return reinterpret_cast<Type*>(output) + 1;
    }
};

////////////////////////////////////////////////////////////////////////////////

// List/deque type encoding
template <template <typename T> class Container, typename Type>
requires (std::is_same_v<Container<Type>, std::list<Type>> || std::is_same_v<Container<Type>, std::deque<Type>>)
struct TMessageEncoder<Container<Type>> {
    static size_t Size(const Container<Type>& data) {
        size_t result = alignof(size_t);

        for (const auto& element : data) {
            result += TMessageEncoder<Type>::Size(element);
        }

        return result;
    }

    static void* Decode(Container<Type>& value, void* mem) {
        size_t size = *reinterpret_cast<size_t*>(mem);
        THROW_EXCEPTION_IF(size > INT32_MAX, "Message format is incorrect");
        
        void* pos = reinterpret_cast<char*>(mem) + alignof(size_t);

        for (int i = 0; i < size; i++) {
            pos = TMessageEncoder<Type>::Decode(value.emplace_back(), pos);
        }

        return pos;
    }

    static void* Encode(const Container<Type>& data, void* output) {
        *reinterpret_cast<size_t*>(output) = data.size();
        void* pos = reinterpret_cast<char*>(output) + alignof(size_t);
        
        for (const auto& element : data) {
            pos = TMessageEncoder<Type>::Encode(element, pos);
        }

        return pos;
    }
};

////////////////////////////////////////////////////////////////////////////////

// Vector type encoding
template <typename Type>
struct TMessageEncoder<std::vector<Type>> {
    static size_t Size(const std::vector<Type>& data) {
        size_t result = alignof(size_t);

        for (const auto& element : data) {
            result += TMessageEncoder<Type>::Size(element);
        }

        return result;
    }

    static void* Decode(std::vector<Type>& value, void* mem) {
        size_t size = *reinterpret_cast<size_t*>(mem);
        THROW_EXCEPTION_IF(size > INT32_MAX, "Message format is incorrect");
        
        value.reserve(size);
        void* pos = reinterpret_cast<char*>(mem) + alignof(size_t);

        for (int i = 0; i < size; i++) {
            pos = TMessageEncoder<Type>::Decode(value.emplace_back(), pos);
        }

        return pos;
    }

    static void* Encode(const std::vector<Type>& data, void* output) {
        *reinterpret_cast<size_t*>(output) = data.size();
        void* pos = reinterpret_cast<char*>(output) + alignof(size_t);
        
        for (const auto& element : data) {
            pos = TMessageEncoder<Type>::Encode(element, pos);
        }

        return pos;
    }
};

////////////////////////////////////////////////////////////////////////////////

// Set encoding
template <template <typename> class Container, typename Type>
requires (std::is_same_v<Container<Type>, std::unordered_set<Type>>
    || std::is_same_v<Container<Type>, std::set<Type>>)
struct TMessageEncoder<Container<Type>> {
    static size_t Size(const Container<Type>& data) {
        size_t result = alignof(size_t);

        for (const auto& element : data) {
            result += TMessageEncoder<Type>::Size(element);
        }

        return result;
    }

    static void* Decode(Container<Type>& value, void* mem) {
        size_t size = *reinterpret_cast<size_t*>(mem);
        THROW_EXCEPTION_IF(size > INT32_MAX, "Message format is incorrect");
        void* pos = reinterpret_cast<char*>(mem) + alignof(size_t);

        for (int i = 0; i < size; i++) {
            Type e;
            pos = TMessageEncoder<Type>::Decode(e, pos);
            value.emplace(e);
        }

        return pos;
    }

    static void* Encode(const Container<Type>& data, void* output) {
        *reinterpret_cast<size_t*>(output) = data.size();
        void* pos = reinterpret_cast<char*>(output) + alignof(size_t);
        
        for (const auto& element : data) {
            pos = TMessageEncoder<Type>::Encode(element, pos);
        }

        return pos;
    }
};

////////////////////////////////////////////////////////////////////////////////

// String encoding
template <typename Char>
struct TMessageEncoder<std::basic_string<Char>> {
    static size_t Size(const std::basic_string<Char>& data) {
        return alignof(size_t) + data.size() * alignof(Char);
    }

    static void* Decode(std::basic_string<Char>& value, void* mem) {
        size_t* pos = reinterpret_cast<size_t*>(mem);
        THROW_EXCEPTION_IF(*pos > INT32_MAX, "Message format is incorrect");

        value.resize(*pos, ' ');
        std::memcpy(value.data(), pos + 1, value.size() * alignof(Char));

        return reinterpret_cast<Char*>(pos + 1) + value.size();
    }

    static void* Encode(const std::basic_string<Char>& data, void* output) {
        *reinterpret_cast<size_t*>(output) = data.size();
        char* pos = reinterpret_cast<char*>(output) + alignof(size_t);
        
        std::memcpy(pos, data.data(), data.size());
        return pos + data.size();
    }
};

////////////////////////////////////////////////////////////////////////////////

// Pair encoding
template <typename A, typename B>
struct TMessageEncoder<std::pair<A, B>> {
    static size_t Size(const std::pair<A, B>& value) {
        return TMessageEncoder<A>::Size(value.first) + TMessageEncoder<B>::Size(value.second);
    }

    static void* Decode(std::pair<A, B>& value, void* data) {
        data = TMessageEncoder<A>::Decode(value.first, data);
        data = TMessageEncoder<B>::Decode(value.second, data);
        return data;
    }

    static void* Encode(const std::pair<A, B>& value, void* output) {
        output = TMessageEncoder<A>::Encode(value.first, output);
        output = TMessageEncoder<B>::Encode(value.second, output);
        return output;
    }
};


////////////////////////////////////////////////////////////////////////////////

// Tuple encoding
template <typename... Args>
struct TMessageEncoder<std::tuple<Args...>> {
    using tuple = std::tuple<Args...>;

    template <size_t index>
    using element = std::tuple_element_t<index, tuple>;

    template <size_t index>
    requires (index != std::tuple_size_v<tuple>)
    static size_t SizeImpl(const tuple& data) {
        return TMessageEncoder<element<index>>::Size(std::get<index>(data)) + SizeImpl<index + 1>(data);
    }

    template <size_t index>
    requires (index == std::tuple_size_v<tuple>)
    static size_t SizeImpl(const tuple& data) {
        return 0;
    }

    static size_t Size(const tuple& data) {
        return SizeImpl<0>(data);
    }
    
    template <size_t index>
    requires (index != std::tuple_size_v<tuple>)
    static void* DecodeImpl(tuple& output, void* data) {
        std::get<index>(output) = TMessageEncoder<element<index>>::Decode(data, &data);
        return DecodeImpl<index + 1>(output, data);
    }
    
    template <size_t index>
    requires (index == std::tuple_size_v<tuple>)
    static void* DecodeImpl(tuple& output, void* data) {
        return data;
    }

    static void* Decode(tuple& value, void* mem) {
        return DecodeImpl<0>(value, mem);
    }

    template <size_t index>
    requires (index != std::tuple_size_v<tuple>)
    static void* EncodeImpl(const tuple& data, void* output) {
        return EncodeImpl<index + 1>(data, TMessageEncoder<element<index>>::Encode(std::get<index>(data), output));
    }

    template <size_t index>
    requires (index == std::tuple_size_v<tuple>)
    static void* EncodeImpl(const tuple& data, void* output) {
        return output;
    }

    static void* Encode(const tuple& data, void* output) {
        return EncodeImpl<0>(data, output);
    }
};


////////////////////////////////////////////////////////////////////////////////

// Map objects
template <template <typename, typename> class Container, typename Key, typename Value>
requires (std::is_same_v<Container<Key, Value>, std::unordered_map<Key, Value>>
    || std::is_same_v<Container<Key, Value>, std::map<Key, Value>>)
struct TMessageEncoder<Container<Key, Value>> {
    static size_t Size(const Container<Key, Value>& data) {
        size_t result = alignof(size_t);

        for (const auto& [key, value] : data) {
            result += TMessageEncoder<Key>::Size(key);
            result += TMessageEncoder<Value>::Size(value);
        }

        return result;
    }

    static void* Decode(Container<Key, Value>& value, void* mem) {
        size_t size = *reinterpret_cast<size_t*>(mem);
        void* pos = reinterpret_cast<char*>(mem) + alignof(size_t);
        THROW_EXCEPTION_IF(size > INT32_MAX, "Message format is incorrect");
        
        for (int i = 0; i < size; i++) {
            std::pair<Key, Value> element;
            pos = TMessageEncoder<Key>::Decode(element.first, pos);
            pos = TMessageEncoder<Value>::Decode(element.second, pos);
            value.emplace(std::move(element));
        }

        return pos;
    }

    static void* Encode(const Container<Key, Value>& data, void* output) {
        *reinterpret_cast<size_t*>(output) = data.size();
        void* pos = reinterpret_cast<char*>(output) + alignof(size_t);
        
        for (const auto& [key, value] : data) {
            pos = TMessageEncoder<Key>::Encode(key, pos);
            pos = TMessageEncoder<Value>::Encode(value, pos);
        }

        return pos;
    }
};

////////////////////////////////////////////////////////////////////////////////

template <typename Type>
void* EncodeToArray(const Type* value, size_t size, void* output) {
    for (int i = 0; i < size; i++) {
        output = TMessageEncoder<Type>::Encode(value[i], output);
    }
    return output;
}

template <typename Type>
void* DecodeArray(Type* value, size_t size, void* input) {
    for (int i = 0; i < size; i++) {
        input = TMessageEncoder<Type>::Decode(value[i], input);
    }
    return input;
}

////////////////////////////////////////////////////////////////////////////////
