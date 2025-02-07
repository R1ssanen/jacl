#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>

#define _J_LOG_MSG(level, msg, ...)                                                                \
    do {                                                                                           \
        fputs(#level ": ", stderr);                                                                \
        fprintf(stderr, msg, ##__VA_ARGS__);                                                       \
    } while (0);

#define J_TRACE(msg, ...) _J_LOG_MSG(jtrace, msg, ##__VA_ARGS__)
#define J_INFO(msg, ...)  _J_LOG_MSG(jinfo, msg, ##__VA_ARGS__)
#define J_WARN(msg, ...)  _J_LOG_MSG(jwarning, msg, ##__VA_ARGS__)
#define J_ERROR(msg, ...) _J_LOG_MSG(jerror, msg, ##__VA_ARGS__)
#define J_FATAL(msg, ...) _J_LOG_MSG(jfatal, msg, ##__VA_ARGS__)

#endif
