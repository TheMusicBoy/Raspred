#pragma once

#include <exception.h>
#include <file.h>

#include <filesystem>
#include <string>

#include <sys/file.h>
#include <unistd.h>

namespace NCache {

////////////////////////////////////////////////////////////////////////////////

template <typename T>
concept Serializable = requires(T t, std::string s)
{
    { s = t.Serialize() };
    { t.Deserialize(s) };
};


template <typename TCache>
requires(Serializable<TCache>)
class TPersistentStorage {
public:
    TPersistentStorage(const std::filesystem::path& storagePath)
        : File_(storagePath.lexically_normal())
    {}

    void Store(const TCache& cache) {
        try {
            File_.Write(cache.Serialize(), "w");
        } catch (std::exception& ex) {
            THROW_EXCEPTION("Failed to store data to persistent storage / {}", ex);
        }
    }
    
    void Load(TCache* cache) {
        try {
            std::string data;
            File_.Read(&data);
            cache->Deserialize(data);
        } catch (std::exception& ex) {
            THROW_EXCEPTION("Failed to load data from persistent storage / {}", ex);
        }
    }
    
    void Lock() {
        try {
            File_.Lock();
        } catch (std::exception& ex) {
            THROW_EXCEPTION("Failed to lock persistent storage / {}", ex);
        }
    }

    void Unlock() {
        try {
            File_.Unlock();
        } catch (std::exception& ex) {
            THROW_EXCEPTION("Failed to unlock persistent storage / {}", ex);
        }
    }

private:
    NFile::TFile File_;

};

////////////////////////////////////////////////////////////////////////////////

} // namespace NCache
