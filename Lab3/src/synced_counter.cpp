#include <synced_counter.h>

namespace NCounter {

////////////////////////////////////////////////////////////////////////////////

std::string TCounter::Serialize() const {
    return std::to_string(counter);
}

void TCounter::Deserialize(const std::string& data) {
    if (data.empty()) {
        counter = 20;
    } else {
        counter = std::stoi(data);
    }
}

////////////////////////////////////////////////////////////////////////////////

TSyncedCounter::TSyncedCounter(const std::filesystem::path& filepath)
    : Storage_(filepath)
{}

TSyncedCounter& TSyncedCounter::operator=(int value) {
    TCounter counter;
    auto guard = NGuard::Guard(Storage_);
    Storage_.Load(&counter);
    counter.counter = value;
    Storage_.Store(counter);
}

TSyncedCounter& TSyncedCounter::operator+=(int value) {
    TCounter counter;
    auto guard = NGuard::Guard(Storage_);
    Storage_.Load(&counter);
    counter.counter += value;
    Storage_.Store(counter);
}

TSyncedCounter& TSyncedCounter::operator++() {
    TCounter counter;
    auto guard = NGuard::Guard(Storage_);
    Storage_.Load(&counter);
    counter.counter++;
    Storage_.Store(counter);
    return *this;
}

TSyncedCounter& TSyncedCounter::operator*=(int value) {
    TCounter counter;
    auto guard = NGuard::Guard(Storage_);
    Storage_.Load(&counter);
    counter.counter *= value;
    Storage_.Store(counter);
    return *this;
}

TSyncedCounter& TSyncedCounter::operator/=(int value) {
    TCounter counter;
    auto guard = NGuard::Guard(Storage_);
    Storage_.Load(&counter);
    counter.counter /= value;
    Storage_.Store(counter);
    return *this;
}

TSyncedCounter::operator int() {
    TCounter counter;
    auto guard = NGuard::Guard(Storage_);
    Storage_.Load(&counter);
    return counter.counter;
}


////////////////////////////////////////////////////////////////////////////////

} // namespace NCounter
