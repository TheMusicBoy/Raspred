#pragma once

#include <string>
#include <sstream>
#include <vector>

namespace NFormat {

namespace {

////////////////////////////////////////////////////////////////////////////////

void Insert(std::ostringstream& out, const std::string& str, int pos)
{
    for (; pos < str.size(); pos++) {
        out.put(str[pos]);
    }
}

template <typename Type, typename... Args>
void Insert(std::ostringstream& out, const std::string& str, int pos, const Type& arg, const Args&... args)
{
    for (; pos < str.size(); pos++) {
        if (pos + 1 < str.size() && str[pos] == '{' && str[pos + 1] == '}') {
            out << arg;
            Insert(out, str, pos + 2, args...);
            return;
        }
        out.put(str[pos]);
    }
}

////////////////////////////////////////////////////////////////////////////////

} // namespace

////////////////////////////////////////////////////////////////////////////////

template <typename... Args>
std::string Format(const std::string& str, const Args&... args) {
    std::ostringstream ss;
    Insert(ss, str, 0, args...);
    return ss.str();
}

template <typename Type>
std::ostream& operator<<(std::ostream& out, const std::vector<Type>& obj) {
    out << "{ ";
    for (auto iter_ = obj.begin(); iter_ != obj.end(); iter_++) {
        if (iter_ != obj.begin()) {
            out << ", ";
        }
        out << *iter_;
    }
    out << " }";
    return out;
}

////////////////////////////////////////////////////////////////////////////////

} // namespace NFormat
