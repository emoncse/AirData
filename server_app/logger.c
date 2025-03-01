#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/stat.h>
#include "logger.h"

static FILE *log_file = NULL;

void ensure_log_directory(const char *dir);
FILE *open_log_file(const char *filename);
void write_log(const char *level, const char *format, va_list args);

void init_logger(const char *filename) {
    ensure_log_directory("logs");
    log_file = open_log_file(filename);
}

void ensure_log_directory(const char *dir) {
    struct stat st = {0};
    if (stat(dir, &st) == -1) {
        mkdir(dir, 0777);
    }
}

FILE *open_log_file(const char *filename) {
    FILE *file = fopen(filename, "a");
    if (!file) {
        perror("Failed to open log file");
        exit(EXIT_FAILURE);
    }
    return file;
}

void log_message(const char *level, const char *format, ...) {
    if (!log_file) return;

    va_list args;
    va_start(args, format);
    write_log(level, format, args);
    va_end(args);
}

void write_log(const char *level, const char *format, va_list args) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    fprintf(log_file, "[%02d-%02d-%d %02d:%02d:%02d] [%s] ", 
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec, level);
    vfprintf(log_file, format, args);
    fprintf(log_file, "\n");

    fflush(log_file);
}

void log_info(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_message("INFO", format, args);
    va_end(args);
}

void log_error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    log_message("ERROR", format, args);
    va_end(args);
}
