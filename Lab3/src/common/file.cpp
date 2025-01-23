#include <common/file.h>
#include <iostream>

#include <sys/file.h>
#include <unistd.h>

namespace NFile {

namespace {

////////////////////////////////////////////////////////////////////////////////

// To close descriptor on exceptions (we don't want system object leaks)
struct TDescHolder {
    TDescHolder(int value)
        : Desc_(value)
    {}

    ~TDescHolder() {
        if (Desc_) {
            close(Desc_);
            Desc_ = 0;
        }
    }

    int Release() {
        int temp = Desc_;
        Desc_ = 0;
        return temp;
    }

    int Desc_;
};

struct TFileDescHolder {
    TFileDescHolder(FILE* value)
        : Desc_(value)
    {}

    ~TFileDescHolder() {
        if (Desc_) {
            fclose(Desc_);
            Desc_ = nullptr;
        }
    }

    FILE* Release() {
        FILE* temp = Desc_;
        Desc_ = nullptr;
        return temp;
    }

    FILE* Desc_;
};

////////////////////////////////////////////////////////////////////////////////

} // namespace

////////////////////////////////////////////////////////////////////////////////

TLockingFile::TLockingFile() = default;

TLockingFile::TLockingFile(const std::filesystem::path& filepath)
    : FilePath_(filepath)
{}

TLockingFile::~TLockingFile() {
    auto guard = std::lock_guard(Mutex_);
    if (LockFileDesc_) {
        Unlock();
    }
}

TLockingFile& TLockingFile::operator=(TLockingFile&& other) {
    Move(&other);
    return *this;
}

void TLockingFile::Move(TLockingFile* other) {
    if (other != this) {
        Unlock();
        LockFileDesc_ = other->LockFileDesc_;
        FilePath_ = std::move(other->FilePath_);

        other->LockFileDesc_ = 0;
    }
}

bool TLockingFile::Valid() const {
    return !FilePath_.empty();
}

void TLockingFile::Lock() {
    auto guard = std::lock_guard(Mutex_);

    if (IsLocked()) {
        return;
    }

    std::filesystem::create_directory(FilePath_.parent_path());
    TDescHolder TempFileDesc_ = open(FilePath_.c_str(), O_CREAT | O_RDONLY, S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP | S_IRUSR | S_IWUSR);
    THROW_EXCEPTION_IF(TempFileDesc_.Desc_ < 0, "Failed to lock file / Failed to create file descriptor (Errno: {}, FilePath: {})", errno, FilePath_);
    THROW_EXCEPTION_IF(flock(TempFileDesc_.Desc_, LOCK_EX), "Failed to lock file (Errno: {}, FilePath: {})", errno, FilePath_);
    LockFileDesc_ = TempFileDesc_.Release();
}

bool TLockingFile::TryLock() {
    auto guard = std::lock_guard(Mutex_);

    if (IsLocked()) {
        return true;
    }

    std::filesystem::create_directory(FilePath_.parent_path());
    TDescHolder TempFileDesc_ = open(FilePath_.c_str(), O_CREAT | O_RDONLY, S_IROTH | S_IWOTH | S_IRGRP | S_IWGRP | S_IRUSR | S_IWUSR);
    THROW_EXCEPTION_IF(TempFileDesc_.Desc_ < 0, "Failed to lock file / Failed to create file descriptor (Errno: {}, FilePath: {})", errno, FilePath_);
    if (flock(TempFileDesc_.Desc_, LOCK_EX | LOCK_NB)) {
        return false;
    }

    LockFileDesc_ = TempFileDesc_.Release();
    return true;
}

void TLockingFile::Unlock() {
    auto guard = std::lock_guard(Mutex_);

    if (!LockFileDesc_) {
        return;
    }

    THROW_EXCEPTION_IF(flock(LockFileDesc_, LOCK_UN), "Failed to unlock file (Errno: {}, FilePath: {})", errno, FilePath_);
    THROW_EXCEPTION_IF(close(LockFileDesc_) < 0, "Failed to close file descriptor (Errno: {}, FilePath: {})", errno, FilePath_);
    LockFileDesc_ = 0;
}

bool TLockingFile::IsLocked() const {
    return LockFileDesc_;
}

////////////////////////////////////////////////////////////////////////////////

TFile& TFile::operator=(TFile&& other) {
    Move(&other);
    return *this;
}

void TFile::Write(const std::string& data, const char* mode) {
    auto guard = std::lock_guard(Mutex_);

    std::filesystem::create_directory(FilePath_.parent_path());

    TFileDescHolder WriteFileDesc_ = fopen(FilePath_.c_str(), mode);
    THROW_EXCEPTION_UNLESS(WriteFileDesc_.Desc_, "Failed to write / Failed to create file descriptor (Errno: {}, FilePath: {})", errno, FilePath_);
    fprintf(WriteFileDesc_.Desc_, "%s", data.data());
}

void TFile::Read(std::string* output) {
    auto guard = std::lock_guard(Mutex_);

    FILE* ReadFileDesc_ = fopen(FilePath_.c_str(), "r");
    THROW_EXCEPTION_UNLESS(ReadFileDesc_, "Failed to read / Failed to create file descriptor (Errno: {}, FilePath: {})", errno, FilePath_);

    fseek(ReadFileDesc_, 0, SEEK_END); 
    long long fileSize = ftell(ReadFileDesc_);
    fseek(ReadFileDesc_, 0, SEEK_SET); 

    output->resize(fileSize);
    fread(output->data(), fileSize, 1, ReadFileDesc_);
}

////////////////////////////////////////////////////////////////////////////////

} // namespace NFile
