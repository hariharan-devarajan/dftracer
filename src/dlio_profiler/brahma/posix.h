//
// Created by hariharan on 8/8/22.
//

#ifndef DLIO_PROFILER_POSIX_H
#define DLIO_PROFILER_POSIX_H

#include <brahma/brahma.h>
#include <dlio_profiler/macro.h>
#include <vector>
#include <dlio_profiler/dlio_logger.h>
#include <fcntl.h>
#include <sys/param.h>
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

namespace brahma {
class POSIXDLIOProfiler : public POSIX {
 private:
  static std::shared_ptr<POSIXDLIOProfiler> instance;
  std::unordered_set<int> tracked_fd;
  std::vector<std::string> track_filename;
  std::shared_ptr<DLIOLogger> logger;
  inline std::string get_filename(int fd) {
    const int kMaxSize = 16*1024;
    char proclnk[kMaxSize];
    char filename[kMaxSize];
    snprintf(proclnk, kMaxSize, "/proc/self/fd/%d", fd);
    size_t r = readlink(proclnk, filename, kMaxSize);
    filename[r] = '\0';
    return filename;
  }
  inline bool is_traced(int fd) {
    auto iter = tracked_fd.find(fd);
    if (iter != tracked_fd.end()) return true;
    return is_traced(get_filename(fd));
  }

  inline bool is_traced(std::string filename) {
    std::ifstream test(filename);
    if (test) {
      auto abs_file = fs::absolute(filename).string();
      for (const auto file : track_filename) {
        if (abs_file.rfind(file, 0) == 0) {
          DLIO_PROFILER_LOGINFO("Profiler Intercepted POSIX tracing %s", filename.c_str());
          return true;
        }
      }
    }
    DLIO_PROFILER_LOGINFO("Profiler Intercepted POSIX not tracing %s", filename.c_str());
    return false;
  }
  inline void trace(int fd) {
    tracked_fd.insert(fd);
  }
  inline void remove_trace(int fd) {
    tracked_fd.erase(fd);
  }
 public:
  POSIXDLIOProfiler() : POSIX() {
    DLIO_PROFILER_LOGINFO("POSIX class intercepted", "");
    logger = DLIO_LOGGER_INIT();
  }
  inline void trace(std::string filename) {
    auto abs_file = fs::absolute(filename).string();
    track_filename.push_back(abs_file);
  }
  ~POSIXDLIOProfiler() override = default;
  static std::shared_ptr<POSIXDLIOProfiler> get_instance() {
    if (instance == nullptr) {
      instance = std::make_shared<POSIXDLIOProfiler>();
      POSIX::set_instance(instance);
    }
    return instance;
  }
  int open(const char *pathname, int flags, mode_t mode) override;
  int creat64(const char *path, mode_t mode) override;
  int open64(const char *path, int flags, mode_t mode) override;
  int close(int fd) override;
  ssize_t write(int fd, const void *buf, size_t count) override;
  ssize_t read(int fd, void *buf, size_t count) override;
  off_t lseek(int fd, off_t offset, int whence) override;
  off64_t lseek64(int fd, off64_t offset, int whence) override;
  ssize_t pread(int fd, void *buf, size_t count, off_t offset) override;
  ssize_t pread64(int fd, void *buf, size_t count, off64_t offset) override;
  ssize_t pwrite(int fd, const void *buf, size_t count, off64_t offset) override;
  ssize_t pwrite64(int fd, const void *buf, size_t count, off64_t offset) override;
  int fsync(int fd) override;
  int fdatasync(int fd) override;

  int openat(int dirfd, const char *pathname, int flags, mode_t mode) override;

  void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) override;

  void *mmap64(void *addr, size_t length, int prot, int flags, int fd, off64_t offset) override;

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

  int linkat(int fd1, const char *path1, int fd2, const char *path2, int flag) override;

  int unlink(const char *pathname) override;

  int symlink(const char *path1, const char *path2) override;

  int symlinkat(const char *path1, int fd, const char *path2) override;

  ssize_t readlink(const char *path, char *buf, size_t bufsize) override;

  ssize_t readlinkat(int fd, const char *path, char *buf, size_t bufsize) override;

  int rename(const char *oldpath, const char *newpath) override;

  int chmod(const char *path, mode_t mode) override;

  int chown(const char *path, uid_t owner, gid_t group) override;

  int lchown(const char *path, uid_t owner, gid_t group) override;

  int utime(const char *filename, const utimbuf *buf) override;

  DIR *opendir(const char *name) override;

  int fcntl(int fd, int cmd, long arg) override;

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
#endif  // DLIO_PROFILER_POSIX_H
