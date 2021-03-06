#ifndef UNTITLED2_SIGLOG_H
#define UNTITLED2_SIGLOG_H

namespace siglog {

    // Types
    enum siglog_level_t {
        DISABLED, MAX, STANDARD, MIN
    };
    typedef enum siglog_level_t LEVEL;
    typedef void (*DUMP_FUNCTION)(FILE *file);

    // Lifecycle functions
    int init(int level_sig, int dump_sig, LEVEL level, const char *dir_path);
    void free();

    // Logging functions
    void log(LEVEL level, char *fmt, ...);
    void min(const char *fmt, ...);
    void standard(const char *fmt, ...);
    void max(const char *fmt, ...);

    // Dumping functions
    int register_dump_function(DUMP_FUNCTION fun);
}

#endif
