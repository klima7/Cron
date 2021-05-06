/*
 * Created by Lukasz Klimkiewicz
 * Index number 222467
 * Date 14.04.2021
 */

#include <cstdio>
#include <pthread.h>
#include <semaphore.h>
#include <csignal>
#include <string>
#include <cstdarg>
#include <ctime>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include "siglog.h"

#define DATE_FORMAT "%m/%d/%y %H:%M:%S"

namespace siglog {

    using namespace std;

// Global variables
    static int initialized;
    static char logging_directory[40];
    static LEVEL current_logging_level;
    static int level_signal, dump_signal;
    static pthread_t level_tid, dump_tid;
    static FILE *log_file;

// Variables to handle dump operation
    static DUMP_FUNCTION *dump_functions;
    static int dump_functions_size;
    static int dump_functions_capacity;

// Variables to ensure threadsafety of operations
    static pthread_mutex_t log_mutex;
    static pthread_mutex_t dump_mutex;

// Prototypes of file scoped functions
    static void *level_thread(void *arg);

    static void level_handler(int signo, siginfo_t *info, void *other);

    static void *dump_thread(void *arg);

    static void dump_handler(int signo, siginfo_t *info, void *other);

    static FILE *create_dump_file();

    static string str_level(LEVEL lvl);

    static void vlog(LEVEL level, const char *fmt, va_list vargs);

// Variables to synchronize level signal handling
    static sem_t sem_level;
    static volatile int level_signal_val;

// Low level signal handler responsible for changing logging level (including disabling)
    static void level_handler(int signo, siginfo_t *info, void *other) {
        level_signal_val = info->si_value.sival_int;
        sem_post(&sem_level);
    }

// High level signal handler responsible for changing logging level
    static void *level_thread(void *arg) {
        // Set thread mask
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, level_signal);
        pthread_sigmask(SIG_UNBLOCK, &set, nullptr);

        // Wait for level change signal and handle it
        while (1) {
            sem_wait(&sem_level);

            if (level_signal_val == -1)
                continue;

            if (level_signal_val >= DISABLED && level_signal_val <= MIN) {
                pthread_mutex_lock(&log_mutex);
                current_logging_level = (LEVEL) level_signal_val;
                pthread_mutex_unlock(&log_mutex);
            }

            level_signal_val = -1;
        }
    }

// Semaphore to synchronize dump signal handling
    static sem_t sem_dump;

// Low level signal handler responsible for dump operation
    static void dump_handler(int signo, siginfo_t *info, void *other) {
        sem_post(&sem_dump);
    }

// High level signal handler responsible for dump operation
    static void *dump_thread(void *arg) {
        // Set thread mask
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, dump_signal);
        pthread_sigmask(SIG_UNBLOCK, &set, nullptr);

        // Wait for dump signal and handle it
        while (1) {
            sem_wait(&sem_dump);
            FILE *dump_file = create_dump_file();
            for (int i = 0; i < dump_functions_size; i++)
                dump_functions[i](dump_file);
            fclose(dump_file);
        }
    }

// Auxiliary function creating dump file in appropriate directory with appropriate name and returning file handler
    static FILE *create_dump_file() {
        char time_buffer[40];

        // Get date
        time_t time_raw = time(nullptr);
        struct tm *time_info = localtime(&time_raw);
        strftime(time_buffer, 40, "%m.%d.%y-%H:%M:%S", time_info);

        // Construct filename
        char filename[50];
        sprintf(filename, "DUMP-%s", time_buffer);

        // Construct path
        char path[100];
        if (logging_directory != nullptr) sprintf(path, "%s/%s", logging_directory, filename);
        else sprintf(path, "%s", filename);

        // Open file
        FILE *file = fopen(path, "w");
        assert(file != nullptr);

        // Write header line to dump file
        fprintf(file, "%s\n", filename);
        fprintf(file, "---------------------\n");

        return file;
    }

/*
 * Function initializing logging library. Must be called before other functions
 * Return value: 0 in case of success, else -1
 *
 * level_sig - signal number to change logging level or disable logging
 * dump_sig - signal number to perform dump operation
 * level - initial logging level
 * path - path to existing directory where log and dump files should appear. nullptr if current directory
 */
    int init(int level_sig, int dump_sig, LEVEL level, const char *path) {

        // Local variables
        sigset_t set;
        struct sigaction act;
        int err;

        if (initialized == 1)
            return -1;

        // Init some global vars
        dump_functions_capacity = 4;
        current_logging_level = level;
        strcpy(logging_directory, path);
        level_signal = level_sig;
        dump_signal = dump_sig;
        level_signal_val = -1;

        // Open file
        char filename[100] = {};
        if (path != nullptr) sprintf(filename, "%s/%s", path, "logs");
        else sprintf(filename, "logs");
        log_file = fopen(filename, "a+");
        if (log_file == nullptr) return -1;

        // Init semaphors
        err = sem_init(&sem_dump, 0, 0);
        if (err != 0) {
            fclose(log_file);
            return -1;
        }

        err = sem_init(&sem_level, 0, 0);
        if (err != 0) {
            sem_destroy(&sem_dump);
            fclose(log_file);
            return -1;
        }

        // Init mutexes
        err = pthread_mutex_init(&log_mutex, nullptr);
        if (err != 0) {
            sem_destroy(&sem_dump);
            sem_destroy(&sem_level);
            fclose(log_file);
            return -1;
        }

        err = pthread_mutex_init(&dump_mutex, nullptr);
        if (err != 0) {
            sem_destroy(&sem_dump);
            sem_destroy(&sem_level);
            fclose(log_file);
            pthread_mutex_destroy(&log_mutex);
            return -1;
        }

        // Set first signal handler
        sigfillset(&set);
        act.sa_sigaction = level_handler;
        act.sa_mask = set;
        act.sa_flags = SA_SIGINFO;
        err = sigaction(level_sig, &act, nullptr);
        if (err != 0) {
            sem_destroy(&sem_dump);
            sem_destroy(&sem_level);
            fclose(log_file);
            pthread_mutex_destroy(&log_mutex);
            pthread_mutex_destroy(&dump_mutex);
            return -1;
        }

        // Set second signal handler
        sigfillset(&set);
        act.sa_sigaction = dump_handler;
        act.sa_mask = set;
        err = sigaction(dump_sig, &act, nullptr);
        if (err != 0) {
            sem_destroy(&sem_dump);
            sem_destroy(&sem_level);
            fclose(log_file);
            pthread_mutex_destroy(&log_mutex);
            pthread_mutex_destroy(&dump_mutex);
            return -1;
        }

        // Create first thread
        err = pthread_create(&level_tid, nullptr, level_thread, nullptr);
        if (err != 0) {
            sem_destroy(&sem_dump);
            sem_destroy(&sem_level);
            fclose(log_file);
            pthread_mutex_destroy(&log_mutex);
            pthread_mutex_destroy(&dump_mutex);
            return -1;
        }

        // Create second thread
        err = pthread_create(&dump_tid, nullptr, dump_thread, nullptr);
        if (err != 0) {
            pthread_cancel(level_tid);
            sem_destroy(&sem_dump);
            sem_destroy(&sem_level);
            fclose(log_file);
            pthread_mutex_destroy(&log_mutex);
            pthread_mutex_destroy(&dump_mutex);
            return -1;
        }

        // Detach first thread
        err = pthread_detach(level_tid);
        if (err != 0) {
            pthread_cancel(level_tid);
            pthread_cancel(dump_tid);
            sem_destroy(&sem_dump);
            sem_destroy(&sem_level);
            fclose(log_file);
            pthread_mutex_destroy(&log_mutex);
            pthread_mutex_destroy(&dump_mutex);
            return -1;
        }

        // Detach second thread
        err = pthread_detach(dump_tid);
        if (err != 0) {
            pthread_cancel(level_tid);
            pthread_cancel(dump_tid);
            sem_destroy(&sem_dump);
            sem_destroy(&sem_level);
            fclose(log_file);
            pthread_mutex_destroy(&log_mutex);
            pthread_mutex_destroy(&dump_mutex);
            return -1;
        }

        // Alloc array of pointers to dump functions
        dump_functions = (DUMP_FUNCTION *) calloc(sizeof(DUMP_FUNCTION), dump_functions_capacity);
        if (dump_functions == nullptr) {
            pthread_cancel(level_tid);
            pthread_cancel(dump_tid);
            sem_destroy(&sem_dump);
            sem_destroy(&sem_level);
            fclose(log_file);
            pthread_mutex_destroy(&log_mutex);
            pthread_mutex_destroy(&dump_mutex);
            return -1;
        }

        initialized = 1;
        return 0;
    }

/*
 * Function disposing all allocated resources
 */
    void free() {
        if (!initialized) return;

        pthread_cancel(level_tid);
        pthread_cancel(dump_tid);
        pthread_join(level_tid, nullptr);
        pthread_join(dump_tid, nullptr);
        sem_destroy(&sem_dump);
        sem_destroy(&sem_level);
        pthread_mutex_destroy(&log_mutex);
        pthread_mutex_destroy(&dump_mutex);
        free(dump_functions);
        fclose(log_file);
    }

// Auxiliary function converting logging level to readable form
    static string str_level(LEVEL lvl) {
        switch (lvl) {
            case MAX:
                return "MAX";
            case STANDARD:
                return "STANDARD";
            case MIN:
                return "MIN";
            default:
                return "";
        }
    }

// Auxiliary logging function accepting va_list
    static void vlog(LEVEL level, const char *fmt, va_list vargs) {
        if (!initialized || level == DISABLED) return;

        if (level <= current_logging_level) {

            // Get date
            time_t time_raw = time(nullptr);
            struct tm *time_info = localtime(&time_raw);
            char time_buffer[50];
            strftime(time_buffer, 50, DATE_FORMAT, time_info);

            // Write to file
            pthread_mutex_lock(&log_mutex);

            fprintf(log_file, "%s | %-8s | ", time_buffer, str_level(level).c_str());
            vfprintf(log_file, fmt, vargs);
            fprintf(log_file, "\n");
            fflush(log_file);

            pthread_mutex_unlock(&log_mutex);
        }

    }

/*
 * Universal function creating log entries with provided logging level
 * level - loggin level
 * fmt, ... - arguments similar to printf
 */
    void log(LEVEL level, const char *fmt, ...) {
        va_list valist;
        va_start(valist, fmt);
        vlog(level, fmt, valist);
        va_end(valist);
    }

/*
 * Function to add log entry with maximum priority
 * fmt, ... - arguments similar to printf
 */
    void max(const char *fmt, ...) {
        va_list valist;
        va_start(valist, fmt);
        vlog(MAX, fmt, valist);
        va_end(valist);
    }

/*
 * Function to add log entry with standard priority
 * fmt, ... - arguments similar to printf
 */
    void standard(const char *fmt, ...) {
        va_list valist;
        va_start(valist, fmt);
        vlog(STANDARD, fmt, valist);
        va_end(valist);
    }

/*
 * Function to add log entry with minimum priority
 * fmt, ... - arguments similar to printf
 */
    void min(const char *fmt, ...) {
        va_list valist;
        va_start(valist, fmt);
        vlog(MIN, fmt, valist);
        va_end(valist);
    }

/*
 * Function responsible for registering dump functions
 * Library user can register as many dump functions as he want. These functions are responsible for supplying data,
 * which will be preserved in dump files In case of multiple dump functions, data is stored in dumps file in order
 * or functions registration.
 *
 * fun - pointer to dump function
 *
 * Return value: 0 in case of success, else -1
 */
    int register_dump_function(DUMP_FUNCTION fun) {
        if (!initialized) return -1;

        pthread_mutex_lock(&dump_mutex);

        if (dump_functions_size >= dump_functions_capacity) {
            DUMP_FUNCTION *new_dump_functions = (DUMP_FUNCTION *) realloc(dump_functions, sizeof(DUMP_FUNCTION) *
                                                                                          dump_functions_capacity * 2);
            if (new_dump_functions == nullptr) return -1;
            dump_functions = new_dump_functions;
            dump_functions_capacity = dump_functions_capacity * 2;
        }
        dump_functions[dump_functions_size++] = fun;

        pthread_mutex_unlock(&dump_mutex);

        return 0;
    }
}