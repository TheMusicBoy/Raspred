#pragma once

#include <common/format.h>

#include <exception>
#include <string>
#include <type_traits>
#include <ostream>

namespace NError {

////////////////////////////////////////////////////////////////////////////////

class TException
    : public std::exception
{
public:
    TException(const std::string& message)
        : Message_(message)
    {}
    
    const char* what() const noexcept override {
        return Message_.c_str();
    }

private:
    std::string Message_;
};

////////////////////////////////////////////////////////////////////////////////

} // namespace NError

template <typename TError>
requires (std::is_base_of_v<std::exception, TError>)
std::ostream& operator<<(std::ostream& os, const TError& error) {
    return os << error.what();
}

#define THROW_EXCEPTION(...) throw NError::TException(NFormat::Format(__VA_ARGS__))

#define THROW_EXCEPTION_IF(condition, ...) if (condition) THROW_EXCEPTION(__VA_ARGS__)

#define THROW_EXCEPTION_UNLESS(condition, ...) THROW_EXCEPTION_IF(!condition, __VA_ARGS__)
