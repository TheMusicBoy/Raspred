#pragma once

#include <common/exception.h>

#include <filesystem>
#include <mutex>

namespace NFile {

////////////////////////////////////////////////////////////////////////////////

class TLockingFile {
public:
    TLockingFile();

    TLockingFile(const std::filesystem::path& filepath);

    TLockingFile(TLockingFile&& file);

    ~TLockingFile();

    TLockingFile& operator=(TLockingFile&& other);

    // This function used to check is there specified file path.
    bool Valid() const;

    // This function used to check if file holding lock.
    bool IsLocked() const;

    void Lock();
    bool TryLock();
    void Unlock();

protected:
    void Move(TLockingFile* other);

    std::filesystem::path FilePath_;
    
    int LockFileDesc_ = 0;
    std::mutex Mutex_;

};

////////////////////////////////////////////////////////////////////////////////

class TFile
    : public TLockingFile
{
public:
    using TLockingFile::TLockingFile;

    TFile& operator=(TFile&& other);

    void Write(const std::string& data, const char* mode);

    void Read(std::string* output);

};

////////////////////////////////////////////////////////////////////////////////

} // namespace NFile
