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
    for(const auto file : track_filename) {
      if (filename.rfind(file, 0) == 0){
        DLIO_PROFILER_LOGINFO("Profiler Intercepted POSIX tracing %s", filename.c_str());
        return true;
      }
    }
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
    track_filename.push_back(filename);
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
};

}  // namespace brahma
#endif  // DLIO_PROFILER_POSIX_H
