//
// Created by hariharan on 8/16/22.
//

#ifndef DLIO_PROFILER_STDIO_H
#define DLIO_PROFILER_STDIO_H

#include <dlio_profiler/utils/utils.h>
#include <brahma/brahma.h>
#include <dlio_profiler/dlio_logger.h>
#include <dlio_profiler/core/macro.h>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

namespace brahma {
    class STDIODLIOProfiler : public STDIO {
    private:
        static std::shared_ptr<STDIODLIOProfiler> instance;
        std::unordered_set<FILE *> tracked_fh;
        std::vector<std::string> track_filename;
        std::vector<std::string> ignore_filename;
        std::shared_ptr<DLIOLogger> logger;

        inline std::pair<bool, std::string> is_traced(FILE *fh, const char *func) {
          if (fh == NULL) return std::pair<bool, std::string>(false, "");
          auto iter = tracked_fh.find(fh);
          if (iter != tracked_fh.end()) return std::pair<bool, std::string>(true, "");
          return is_traced(get_filename(fileno(fh)).c_str(), func);
        }

        inline std::pair<bool, std::string> is_traced(const char *filename, const char *func) {
          return is_traced_common(filename, func, ignore_filename, track_filename);
        }

        inline void trace(FILE *fh) {
          tracked_fh.insert(fh);
        }

        inline void remove_trace(FILE *fh) {
          tracked_fh.erase(fh);
        }

    public:
        STDIODLIOProfiler() : STDIO(), tracked_fh(), track_filename() {
          DLIO_PROFILER_LOGINFO("STDIO class intercepted", "");
          logger = DLIO_LOGGER_INIT();
        }

        inline void trace(const char *filename) {
          char resolved_path[PATH_MAX];
          char *data = realpath(filename, resolved_path);
          (void) data;
          track_filename.push_back(resolved_path);
        }

        inline void untrace(const char *filename) {
          char resolved_path[PATH_MAX];
          char *data = realpath(filename, resolved_path);
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
