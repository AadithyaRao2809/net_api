#ifndef DEBUG_F

#include <string>
#include <iostream>

#define DEBUG_F

template <typename... T>
void debug_log(std::string file, int line, T... args) {
#ifdef DEBUG
    std::clog << "[" << file << ":" << line << "] ";
    ((std::clog << args << " "), ...);
    std::clog << std::endl;

#endif
    return;
}

#define debug(...) debug_log(__FILE__, __LINE__, __VA_ARGS__)
#endif
