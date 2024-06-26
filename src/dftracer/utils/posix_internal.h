//
// Created by haridev on 8/23/23.
//

#ifndef DFTRACER_POSIX_INTERNAL_H
#define DFTRACER_POSIX_INTERNAL_H

#include <dftracer/core/typedef.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <syscall.h>
#include <unistd.h>

#include <csignal>

int df_open(const char *pathname, int flags, ...);

ssize_t df_write(int fd, const void *buf, size_t count);

ssize_t df_read(int fd, void *buf, size_t count);

int df_close(int fd);

int df_fsync(int fd);

ssize_t df_readlink(const char *path, char *buf, size_t bufsize);

int df_unlink(const char *filename);

ThreadID df_gettid();

ProcessID df_getpid();

#endif  // DFTRACER_POSIX_INTERNAL_H
