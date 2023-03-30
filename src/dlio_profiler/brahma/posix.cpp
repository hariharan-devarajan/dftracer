//
// Created by hariharan on 8/16/22.
//
#include <cpp-logger/logger.h>
#include <dlio_profiler/brahma/posix.h>
#include <dlio_profiler/dlio_logger.h>

#define CATEGORY "POSIX"

std::shared_ptr<brahma::POSIXDLIOProfiler> brahma::POSIXDLIOProfiler::instance = nullptr;
int brahma::POSIXDLIOProfiler::open(const char *pathname, int flags, mode_t mode) {
  BRAHMA_MAP_OR_FAIL(open);
  DLIO_LOGGER_START(pathname);
  int ret = __real_open(pathname, flags, mode);
  DLIO_LOGGER_UPDATE(ret);
  DLIO_LOGGER_END();
  if (trace) this->trace(ret);
  return ret;
}
int brahma::POSIXDLIOProfiler::close(int fd) {
  BRAHMA_MAP_OR_FAIL(close);
  DLIO_LOGGER_START(fd);
  int ret = __real_close(fd);
  DLIO_LOGGER_END();
  if (trace) this->remove_trace(fd);
  return ret;
}
ssize_t brahma::POSIXDLIOProfiler::write(int fd, const void *buf, size_t count) {
  BRAHMA_MAP_OR_FAIL(write);
  DLIO_LOGGER_START(fd);
  ssize_t ret = __real_write(fd, buf, count);
  DLIO_LOGGER_END();
  return ret;
}
ssize_t brahma::POSIXDLIOProfiler::read(int fd, void *buf, size_t count) {
  BRAHMA_MAP_OR_FAIL(read);
  DLIO_LOGGER_START(fd);
  ssize_t ret = __real_read(fd, buf, count);
  DLIO_LOGGER_END();
  return ret;
}
off_t brahma::POSIXDLIOProfiler::lseek(int fd, off_t offset, int whence) {
  BRAHMA_MAP_OR_FAIL(lseek);
  DLIO_LOGGER_START(fd);
  ssize_t ret = __real_lseek(fd, offset, whence);
  DLIO_LOGGER_END();
  return ret;
}

int brahma::POSIXDLIOProfiler::creat64(const char *path, mode_t mode) {
  BRAHMA_MAP_OR_FAIL(creat64);
  DLIO_LOGGER_START(path);
  int ret = __real_creat64(path, mode);
  DLIO_LOGGER_END();
  if (trace) this->trace(path);
  return ret;
}

int brahma::POSIXDLIOProfiler::open64(const char *path, int flags, mode_t mode) {
  BRAHMA_MAP_OR_FAIL(open64);
  DLIO_LOGGER_START(path);
  int ret = __real_open64(path, flags, mode);
  DLIO_LOGGER_END();
  if (trace) this->trace(path);
  return ret;
}

off64_t brahma::POSIXDLIOProfiler::lseek64(int fd, off64_t offset, int whence) {
  BRAHMA_MAP_OR_FAIL(lseek64);
  DLIO_LOGGER_START(fd);
  off64_t ret = __real_lseek64(fd, offset, whence);
  DLIO_LOGGER_END();
  return ret;
}

ssize_t brahma::POSIXDLIOProfiler::pread(int fd, void *buf, size_t count, off_t offset) {
  BRAHMA_MAP_OR_FAIL(pread);
  DLIO_LOGGER_START(fd);
  ssize_t ret = __real_pread(fd, buf, count, offset);
  DLIO_LOGGER_END();
  return ret;
}

ssize_t brahma::POSIXDLIOProfiler::pread64(int fd, void *buf, size_t count, off64_t offset) {
  BRAHMA_MAP_OR_FAIL(pread64);
  DLIO_LOGGER_START(fd);
  ssize_t ret = __real_pread64(fd, buf, count, offset);
  DLIO_LOGGER_END();
  return ret;
}

ssize_t brahma::POSIXDLIOProfiler::pwrite(int fd, const void *buf, size_t count, off64_t offset) {
  BRAHMA_MAP_OR_FAIL(pwrite);
  DLIO_LOGGER_START(fd);
  ssize_t ret = __real_pwrite(fd, buf, count, offset);
  DLIO_LOGGER_END();
  return ret;
}

ssize_t brahma::POSIXDLIOProfiler::pwrite64(int fd, const void *buf, size_t count, off64_t offset) {
  BRAHMA_MAP_OR_FAIL(pwrite64);
  DLIO_LOGGER_START(fd);
  ssize_t ret = __real_pwrite64(fd, buf, count, offset);
  DLIO_LOGGER_END();
  return ret;
}

int brahma::POSIXDLIOProfiler::fsync(int fd) {
  BRAHMA_MAP_OR_FAIL(fsync);
  DLIO_LOGGER_START(fd);
  int ret = __real_fsync(fd);
  DLIO_LOGGER_END();
  return ret;
}

int brahma::POSIXDLIOProfiler::fdatasync(int fd) {
  BRAHMA_MAP_OR_FAIL(fdatasync);
  DLIO_LOGGER_START(fd);
  int ret = __real_fdatasync(fd);
  DLIO_LOGGER_END();
  return ret;
}
