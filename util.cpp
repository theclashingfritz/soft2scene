#include "util.h"

int verbose = 3;
FILE *log_file = nullptr;

// General

void safe_exit(int code) {
    // Close the log for debugging.
    if (log_file) {
        fflush(log_file);
        fclose(log_file);
        log_file = nullptr;
    }

    exit(code);
}