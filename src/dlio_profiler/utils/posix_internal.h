//
// Created by haridev on 8/23/23.
//

#ifndef DLIO_PROFILER_POSIX_INTERNAL_H
#define DLIO_PROFILER_POSIX_INTERNAL_H

#include <csignal>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <syscall.h>
#include <dlio_profiler/core/typedef.h>

int dlp_open(const char *pathname, int flags, ...);

ssize_t dlp_write(int fd, const void *buf, size_t count);

ssize_t dlp_read(int fd, void *buf, size_t count);

int dlp_close(int fd);

int dlp_fsync(int fd);

ssize_t dlp_readlink(const char *path, char *buf, size_t bufsize);

int dlp_unlink(const char* filename);

ThreadID dlp_gettid();

ProcessID dlp_getpid();

#endif // DLIO_PROFILER_POSIX_INTERNAL_H
