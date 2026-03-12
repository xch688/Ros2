#include "sylar/util.h"

#include <cstring>
#include <filesystem>
#include <iostream>
#include <syscall.h>
#include <unistd.h>

namespace sylar {

pid_t GetThreadId()
{
    return static_cast<pid_t>(syscall(SYS_gettid));
}

uint32_t GetFiberId()
{
    return 1234;
}


bool FSUtils::OpenForWrite(std::ofstream& ofs, const std::string& filename, std::ios::openmode mode)
{
    ofs.open(filename, mode);
    if (!ofs.is_open()) {
        std::cerr << "failed to open" << filename << ",reason: " << strerror(errno) << std::endl;
        if (const std::string dir = std::filesystem::path(filename).parent_path().string();
            !dir.empty()) {
            std::error_code ec;
            std::filesystem::create_directories(dir, ec);
            if (ec) {
                std::cerr << "Failed to create directory: " << dir << ", error: " << ec.message()
                          << std::endl;
                return false;
            }
        }

        ofs.open(filename, mode);
    }

    return ofs.is_open();
}

}   // namespace sylar
