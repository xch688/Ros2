#pragma once

#include <fstream>
#include <stdint.h>

namespace sylar {

pid_t GetThreadId();
uint32_t GetFiberId();

class FSUtils {
public:
    static bool OpenForWrite(std::ofstream& ofs, const std::string& filename,
                             std::ios::openmode mode = std::ios::out);
};

}   // namespace sylar