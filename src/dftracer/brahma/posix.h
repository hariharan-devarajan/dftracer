//
// Created by hariharan on 8/8/22.
//

#ifndef DFTRACER_POSIX_H
#define DFTRACER_POSIX_H

#include <brahma/brahma.h>
#include <dftracer/core/macro.h>
#include <dftracer/df_logger.h>
#include <dftracer/utils/utils.h>
#include <fcntl.h>
#include <sys/param.h>

#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

namespace brahma {
class POSIXDFTracer : public POSIX {
 private:
  static bool stop_trace;
  static std::shared_ptr<POSIXDFTracer> instance;
  static const int MAX_FD = 1024;
  std::string tracked_fd[MAX_FD];
  std::shared_ptr<DFTLogger> logger;
  bool trace_all_files;

  inline const char *is_traced(int fd, const char *func) {
    if (fd == -1) return nullptr;
    const char *trace = tracked_fd[fd % MAX_FD].empty()
                            ? nullptr
                            : tracked_fd[fd % MAX_FD].c_str();
    if (trace != nullptr)
      DFTRACER_LOGDEBUG(
          "Calling POSIXDFTracer.is_traced for %s and"
          " fd %d trace %d",
          func, fd, trace != nullptr);
    return trace;
  }

  inline const char *is_traced(const char *filename, const char *func) {
    const char *trace = nullptr;
    if (stop_trace) return nullptr;
    if (trace_all_files)
      return filename;
    else
      trace = is_traced_common(filename, func);
    if (trace != nullptr)
      DFTRACER_LOGDEBUG(
          "Calling POSIXDFTracer.is_traced with "
          "filename %s for %s trace %d",
          filename, func, trace != nullptr);
    return trace;
  }

  inline void trace(int fd, const char *filename) {
    DFTRACER_LOGDEBUG("Calling POSIXDFTracer.trace for %d and %s", fd,
                      filename);
    if (fd == -1) return;
    tracked_fd[fd % MAX_FD] = filename;
  }

  inline void remove_trace(int fd) {
    DFTRACER_LOGDEBUG("Calling POSIXDFTracer.remove_trace for %d", fd);
    if (fd == -1) return;
    tracked_fd[fd % MAX_FD] = std::string();
  }

 public:
  POSIXDFTracer(bool trace_all) : POSIX(), trace_all_files(trace_all) {
    DFTRACER_LOGDEBUG("POSIX class intercepted", "");
    for (int i = 0; i < MAX_FD; ++i) tracked_fd[i] = std::string();
    logger = DFT_LOGGER_INIT();
  }
  void finalize() {
    DFTRACER_LOGDEBUG("Finalizing POSIXDFTracer", "");
    stop_trace = true;
  }
  ~POSIXDFTracer() { DFTRACER_LOGDEBUG("Destructing POSIXDFTracer", ""); }
  static std::shared_ptr<POSIXDFTracer> get_instance(bool trace_all = false) {
    DFTRACER_LOGDEBUG("POSIX class get_instance", "");
    if (!stop_trace && instance == nullptr) {
      instance = std::make_shared<POSIXDFTracer>(trace_all);
      POSIX::set_instance(instance);
    }
    return instance;
  }

  int open(const char *pathname, int flags, ...) override;

  int creat64(const char *path, mode_t mode) override;

  int open64(const char *path, int flags, ...) override;

  int close(int fd) override;

  ssize_t write(int fd, const void *buf, size_t count) override;

  ssize_t read(int fd, void *buf, size_t count) override;

  off_t lseek(int fd, off_t offset, int whence) override;

  off64_t lseek64(int fd, off64_t offset, int whence) override;

  ssize_t pread(int fd, void *buf, size_t count, off_t offset) override;

  ssize_t pread64(int fd, void *buf, size_t count, off64_t offset) override;

  ssize_t pwrite(int fd, const void *buf, size_t count,
                 off64_t offset) override;

  ssize_t pwrite64(int fd, const void *buf, size_t count,
                   off64_t offset) override;

  int fsync(int fd) override;

  int fdatasync(int fd) override;

  int openat(int dirfd, const char *pathname, int flags, ...) override;

  void *mmap(void *addr, size_t length, int prot, int flags, int fd,
             off_t offset) override;

  void *mmap64(void *addr, size_t length, int prot, int flags, int fd,
               off64_t offset) override;

  int __xstat(int vers, const char *path, struct stat *buf) override;

  int __xstat64(int vers, const char *path, struct stat64 *buf) override;

  int __lxstat(int vers, const char *path, struct stat *buf) override;

  int __lxstat64(int vers, const char *path, struct stat64 *buf) override;

  int __fxstat(int vers, int fd, struct stat *buf) override;

  int __fxstat64(int vers, int fd, struct stat64 *buf) override;

  int mkdir(const char *pathname, mode_t mode) override;

  int rmdir(const char *pathname) override;

  int chdir(const char *path) override;

  int link(const char *oldpath, const char *newpath) override;

  int linkat(int fd1, const char *path1, int fd2, const char *path2,
             int flag) override;

  int unlink(const char *pathname) override;

  int symlink(const char *path1, const char *path2) override;

  int symlinkat(const char *path1, int fd, const char *path2) override;

  ssize_t readlink(const char *path, char *buf, size_t bufsize) override;

  ssize_t readlinkat(int fd, const char *path, char *buf,
                     size_t bufsize) override;

  int rename(const char *oldpath, const char *newpath) override;

  int chmod(const char *path, mode_t mode) override;

  int chown(const char *path, uid_t owner, gid_t group) override;

  int lchown(const char *path, uid_t owner, gid_t group) override;

  int utime(const char *filename, const utimbuf *buf) override;

  DIR *opendir(const char *name) override;

  int fcntl(int fd, int cmd, ...) override;

  int dup(int oldfd) override;

  int dup2(int oldfd, int newfd) override;

  int mkfifo(const char *pathname, mode_t mode) override;

  mode_t umask(mode_t mask) override;

  int access(const char *path, int amode) override;

  int faccessat(int fd, const char *path, int amode, int flag) override;

  int remove(const char *pathname) override;

  int truncate(const char *pathname, off_t length) override;

  int ftruncate(int fd, off_t length) override;
};

}  // namespace brahma
#endif  // DFTRACER_POSIX_H
