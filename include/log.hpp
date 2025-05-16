#ifndef JACL_LOG_HPP_
#define JACL_LOG_HPP_

#include <format>
#include <iostream>

#define JACL_FORMAT_LOG_ARGS(fmt, ...) std::format(fmt __VA_OPT__(, ) __VA_ARGS__)

#define JLOG(fmt, ...)                                                                             \
    do { std::cout << "jacl <LOG> : " << JACL_FORMAT_LOG_ARGS(fmt, __VA_ARGS__) << '\n'; } while (0)

#define JERROR(fmt, ...)                                                                           \
    do {                                                                                           \
        std::cerr << "jacl <ERROR> : " << JACL_FORMAT_LOG_ARGS(fmt, __VA_ARGS__) << '\n';          \
    } while (0)

#define JWARN(fmt, ...)                                                                            \
    do {                                                                                           \
        std::cerr << "jacl <WARNING> : " << JACL_FORMAT_LOG_ARGS(fmt, __VA_ARGS__) << '\n';        \
    } while (0)

#endif
