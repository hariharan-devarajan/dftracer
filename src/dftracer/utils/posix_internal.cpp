//
// Created by haridev on 8/23/23.
//

#include <dftracer/core/macro.h>
#include <dftracer/utils/posix_internal.h>

#include <thread>

int df_open(const char *pathname, int flags, ...) {
  DFTRACER_LOGDEBUG("df_open", "");
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

  if (result >= 0) return (int)result;
  return -1;  // GCOV_EXCL_LINE
}

ssize_t df_write(int fd, const void *buf, size_t count) {
  DFTRACER_LOGDEBUG("df_write %d %d", fd, count);
  return syscall(SYS_write, fd, buf, count);
}

off_t df_read(int fd, void *buf, size_t count) {
  DFTRACER_LOGDEBUG("df_read", "");
  return syscall(SYS_read, fd, buf, count);
}

int df_close(int fd) {
  DFTRACER_LOGDEBUG("close %d", fd);
  return syscall(SYS_close, fd);
}

int df_unlink(const char *filename) {
  DFTRACER_LOGDEBUG("df_unlink", "");
#if defined(SYS_unlink)
  return syscall(SYS_unlink, filename);
#else
  return syscall(SYS_unlinkat, filename);
#endif
}

int df_fsync(int fd) {  // GCOV_EXCL_START
  DFTRACER_LOGDEBUG("df_fsync", "");
  return syscall(SYS_fsync, fd);
}  // GCOV_EXCL_STOP

ssize_t df_readlink(const char *path, char *buf, size_t bufsize) {
  DFTRACER_LOGDEBUG("df_readlink", "");
#ifdef SYS_readlink
  return syscall(SYS_readlink, path, buf, bufsize);
#else
  return syscall(SYS_readlinkat, path, buf, bufsize);
#endif
}

ThreadID df_gettid() {
  DFTRACER_LOGDEBUG("df_gettid", "");
  return syscall(SYS_gettid);
}

ProcessID df_getpid() {
  DFTRACER_LOGDEBUG("df_getpid", "");
  return syscall(SYS_getpid);
}
