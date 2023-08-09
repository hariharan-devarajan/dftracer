//
// Created by hariharan on 8/16/22.
//

#ifndef DLIO_PROFILER_STDIO_H
#define DLIO_PROFILER_STDIO_H
#include <brahma/brahma.h>
#include <fcntl.h>
#include <dlio_profiler/macro.h>
#include <vector>
#include <dlio_profiler/dlio_logger.h>
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

namespace brahma {
class STDIODLIOProfiler : public STDIO {
 private:
  static std::shared_ptr<STDIODLIOProfiler> instance;
  std::unordered_set<FILE*> tracked_fh;
  std::vector<std::string> track_filename;
  std::vector<std::string> ignore_filename;
  std::shared_ptr<DLIOLogger> logger;
  inline std::string get_filename(int fd) {
    char proclnk[PATH_MAX];
    char filename[PATH_MAX];
    snprintf(proclnk, PATH_MAX, "/proc/self/fd/%d", fd);
    size_t r = readlink(proclnk, filename, PATH_MAX);
    filename[r] = '\0';
    return filename;
  }
  inline std::pair<bool, std::string> is_traced(FILE* fh, const char* func) {
    if (fh == NULL) return std::pair<bool, std::string>(false, "");
    auto iter = tracked_fh.find(fh);
    if (iter != tracked_fh.end()) return std::pair<bool, std::string>(true, "");
    return is_traced(get_filename(fileno(fh)).c_str(), func);
  }
  inline std::pair<bool, std::string> is_traced(const char* filename, const char* func) {
    bool found = false;
    bool ignore = false;
    char resolved_path[PATH_MAX];
    char* data = realpath(filename, resolved_path);
    (void) data;
    if (ignore_files(resolved_path)) {
        DLIO_PROFILER_LOGINFO("Profiler ignoring logfile %s", resolved_path);
        return std::pair<bool, std::string>(false, filename);
    }
    for (const auto file : ignore_filename) {
      if (strstr(resolved_path, file.c_str()) != NULL) {
        DLIO_PROFILER_LOGINFO("Profiler Intercepted POSIX not tracing %s %s %s", resolved_path, filename, func);
        ignore = true;
        break;
      }
    }
    if (!ignore) {
      for (const auto file : track_filename) {
        if (strstr(resolved_path, file.c_str()) != NULL) {
          DLIO_PROFILER_LOGINFO("Profiler Intercepted STDIO tracing %s %s %s", resolved_path, filename, func);
          found = true;
          break;
        }
      }
    }
    DLIO_PROFILER_LOGINFO("Profiler Intercepted STDIO not tracing %s %s %s", resolved_path, filename, func);
    return std::pair<bool, std::string>(found, filename);
  }
  inline void trace(FILE* fh) {
    tracked_fh.insert(fh);
  }
  inline void remove_trace(FILE* fh) {
    tracked_fh.erase(fh);
  }
 public:
  STDIODLIOProfiler() : STDIO(), tracked_fh(), track_filename() {
    DLIO_PROFILER_LOGINFO("STDIO class intercepted", "");
    logger = DLIO_LOGGER_INIT();
  }

  inline void trace(const char* filename) {
    char resolved_path[PATH_MAX];
    char* data = realpath(filename, resolved_path);
    (void) data;
    track_filename.push_back(resolved_path);
  }
  inline void untrace(const char* filename) {
    char resolved_path[PATH_MAX];
    char* data = realpath(filename, resolved_path);
    (void) data;
    ignore_filename.push_back(resolved_path);
  }

  ~STDIODLIOProfiler() = default;
  static std::shared_ptr<STDIODLIOProfiler> get_instance() {
    if (instance == nullptr) {
      instance = std::make_shared<STDIODLIOProfiler>();
      STDIO::set_instance(instance);
    }
    return instance;
  }
  FILE *fopen(const char *path, const char *mode) override;
  FILE *fopen64(const char *path, const char *mode) override;
  int fclose(FILE *fp) override;
  size_t fread(void *ptr, size_t size, size_t nmemb, FILE *fp) override;
  size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *fp) override;
  long ftell(FILE *fp) override;
  int fseek(FILE *fp, long offset, int whence) override;
};

}  // namespace brahma
#endif  // DLIO_PROFILER_STDIO_H
