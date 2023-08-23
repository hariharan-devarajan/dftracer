//
// Created by haridev on 3/28/23.
//

#ifndef DLIO_PROFILER_BASE_WRITER_H
#define DLIO_PROFILER_BASE_WRITER_H

#include <any>
#include <csignal>
#include <dlio_profiler/core/common.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <syscall.h>
#include <unordered_map>
namespace dlio_profiler {
    class BaseWriter {
    private:
        std::unordered_map<char *, std::any> metadata;
    protected:
        bool throw_error;
        std::string filename;
        int dlp_open(const char *pathname, int flags, ...) {
          mode_t mode;
          va_list args;
          long result;

          va_start(args, flags);
          if (flags & O_CREAT) {
            mode = va_arg(args, mode_t);
          }
          else {
            mode = 0;
          }
          va_end(args);
#if defined(SYS_open)
          result = syscall(SYS_open, pathname, flags, mode);
#else
          result = syscall(SYS_openat, AT_FDCWD, pathname, flags, mode);
#endif

          if (result >= 0)
            return (int) result;
          return -1;
        }

        ssize_t dlp_write(int fd, const void *buf, size_t count) {
          return syscall(SYS_write, fd, buf, count);
        }

        ssize_t dlp_read(int fd, void *buf, size_t count) {
          return syscall(SYS_read, fd, buf, count);
        }

        int dlp_close(int fd) {
          return syscall(SYS_close, fd);
        }

        int dlp_fsync(int fd) {
          return syscall(SYS_fsync, fd);
        }
    public:
        virtual void initialize(char *filename, bool throw_error) = 0;

        virtual void log(std::string &event_name, std::string &category,
                         TimeResolution &start_time, TimeResolution &duration,
                         std::unordered_map<std::string, std::any> &metadata, int process_id) = 0;

        virtual void finalize() = 0;

    };
}
#endif //DLIO_PROFILER_BASE_WRITER_H
