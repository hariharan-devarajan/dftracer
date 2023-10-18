//
// Created by haridev on 8/23/23.
//

#include <dlio_profiler/utils/posix_internal.h>
#include <thread>

int dlp_open(const char *pathname, int flags, ...) {
  mode_t mode;
  va_list args;
  long result;

  va_start(args, flags);
  if (flags & O_CREAT) {
    mode = va_arg(args, mode_t);
  } else {
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
  return -1; // GCOV_EXCL_LINE
}

ssize_t dlp_write(int fd, const void *buf, size_t count) {
  return syscall(SYS_write, fd, buf, count);
}

off_t dlp_read(int fd, void *buf, size_t count) {
  return syscall(SYS_read, fd, buf, count);
}

int dlp_close(int fd) {
  return syscall(SYS_close, fd);
}

int dlp_unlink(const char* filename) {
  return syscall(SYS_unlink, filename);
}

int dlp_fsync(int fd) { // GCOV_EXCL_START
  return syscall(SYS_fsync, fd);
} // GCOV_EXCL_STOP

ssize_t dlp_readlink(const char *path, char *buf, size_t bufsize) {
  return syscall(SYS_readlinkat, path, buf, bufsize);
}

ThreadID dlp_gettid(){
  return syscall(SYS_gettid);
}

ProcessID dlp_getpid(){
  return syscall(SYS_getpid);
}
