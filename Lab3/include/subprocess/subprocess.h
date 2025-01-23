#include <string>
#include <future>

#include <functional>

namespace NSubprocess {

////////////////////////////////////////////////////////////////////////////////

template <typename TCallable>
std::future<int> CreateSubprocess(TCallable&& callable) {
    std::promise<int> promise;
    std::future<int> future = promise.get_future();

#if defined(_WIN32) || defined(_WIN64)
    std::unique_ptr<STARTUPINFOA> si = std::make_unique<STARTUPINFOA>();
    std::unqieu_ptr<PROCESS_INFORMATION> pi = std::make_unique<PROCESS_INFORMATION>();

    ZeroMemory(si.get(), sizeof(si));
    si->cb = sizeof(*si);
    ZeroMemory(pi.get(), sizeof(pi));

    if (!CreateProcessA(
            nullptr,
            cmd.c_str(),
            nullptr,
            nullptr,
            FALSE,
            0,
            nullptr,
            nullptr,
            &si,
            &pi
    )) {
        std::cerr << "CreateProcess failed: " << GetLastError() << std::endl;
        promise.set_value(-1);
        return;
    }


    std::thread([cmd, promise = std::move(promise)]() mutable {
        WaitForSingleObject(pi.hProcess, INFINITE);

        DWORD exitCode;
        if (!GetExitCodeProcess(pi.hProcess, &exitCode)) {
            promise.set_value(-1);
        } else {
            promise.set_value(static_cast<int>(exitCode));
        }

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }).detach();
#else
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("failed fork process");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        try {
            std::invoke(callable);
            promise.set_value(0);
            exit(0);
        } catch (std::exception& ex) {
            exit(1);
        }
    }

    return future;
#endif
}

////////////////////////////////////////////////////////////////////////////////

} // namespace NSubprocess
