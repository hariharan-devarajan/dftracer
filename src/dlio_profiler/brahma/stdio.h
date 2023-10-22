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
        static bool stop_trace;
        static std::shared_ptr<STDIODLIOProfiler> instance;
        std::unordered_map<FILE *, std::string> tracked_fh;
        std::shared_ptr<DLIOLogger> logger;
        bool trace_all_files;

        inline const char* is_traced(FILE *fh, const char *func) {
          DLIO_PROFILER_LOGDEBUG("Calling STDIODLIOProfiler.is_traced for %s",func);
          if (fh == NULL) return nullptr;
          auto iter = tracked_fh.find(fh);
          if (iter != tracked_fh.end()) {
            return iter->second.c_str();
          }
          return nullptr;
        }

        inline const char* is_traced(const char *filename, const char *func) {
          DLIO_PROFILER_LOGDEBUG("Calling STDIODLIOProfiler.is_traced with filename for %s",func);
          if (stop_trace) return nullptr;
          if (trace_all_files) return filename;
          else return is_traced_common(filename, func);
        }

        inline void trace(FILE *fh, const char* filename) {
          DLIO_PROFILER_LOGDEBUG("Calling STDIODLIOProfiler.trace with filename", "");
          tracked_fh.insert_or_assign(fh, filename);
        }

        inline void remove_trace(FILE *fh) {
          DLIO_PROFILER_LOGDEBUG("Calling STDIODLIOProfiler.remove_trace with filename", "");
          tracked_fh.erase(fh);
        }

    public:
        STDIODLIOProfiler(bool trace_all) : STDIO(), tracked_fh(), trace_all_files(trace_all)  {
          DLIO_PROFILER_LOGDEBUG("STDIO class intercepted", "");
          logger = DLIO_LOGGER_INIT();
        }
        void finalize() {
          DLIO_PROFILER_LOGDEBUG("Finalizing STDIODLIOProfiler","");
          stop_trace = true;
        }
        ~STDIODLIOProfiler() {
          DLIO_PROFILER_LOGDEBUG("Destructing STDIODLIOProfiler","");
        };

        static std::shared_ptr<STDIODLIOProfiler> get_instance(bool trace_all = false) {
          DLIO_PROFILER_LOGDEBUG("STDIO class get_instance", "");
          if (!stop_trace && instance == nullptr) {
            instance = std::make_shared<STDIODLIOProfiler>(trace_all);
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
