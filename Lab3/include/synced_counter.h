#include <common/log.h>
#include <common/common.h>
#include <cache/cache.h>

#include <filesystem>
#include <atomic>

namespace NCounter {

////////////////////////////////////////////////////////////////////////////////

struct TCounter {
    int counter;

    std::string Serialize() const;

    void Deserialize(const std::string& data);
};

class TSyncedCounter {
public:
    TSyncedCounter(const std::filesystem::path& filepath);

    TSyncedCounter& operator=(int value);

    TSyncedCounter& operator++();

    TSyncedCounter& operator+=(int value);

    TSyncedCounter& operator*=(int value);

    TSyncedCounter& operator/=(int value);

    operator int();

private:
    NCache::TPersistentStorage<TCounter> Storage_;

};

////////////////////////////////////////////////////////////////////////////////

} // namespace NCounter
