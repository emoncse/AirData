#ifndef LOGGER_H
#define LOGGER_H

void init_logger(const char *filename);
void log_info(const char *format, ...);
void log_error(const char *format, ...);

#endif
