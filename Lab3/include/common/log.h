#pragma once

#include <common/format.h>
#include <common/file.h>

#include <filesystem>
#include <string>
#include <stdio.h>

namespace NLogging {

////////////////////////////////////////////////////////////////////////////////

class TLoggingPipe {
public:
    static TLoggingPipe* GetInstance();

    void SetLogFile(const std::filesystem::path& path);

    void Print(const std::string& message);

private:
    TLoggingPipe();

    static TLoggingPipe* Instance_;

    NFile::TFile File_;
    
};

inline TLoggingPipe* TLoggingPipe::Instance_ = nullptr;

////////////////////////////////////////////////////////////////////////////////

} // namespace NLogging

#define LOG(...) NLogging::TLoggingPipe::GetInstance()->Print(NFormat::Format(__VA_ARGS__))
