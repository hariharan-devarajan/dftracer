//
// Created by hariharan on 8/16/22.
//

#ifndef DFTRACER_STDIO_H
#define DFTRACER_STDIO_H

#include <brahma/brahma.h>
#include <dftracer/core/macro.h>
#include <dftracer/df_logger.h>
#include <dftracer/utils/utils.h>
#include <fcntl.h>

#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

namespace brahma {
class STDIODFTracer : public STDIO {
 private:
  static bool stop_trace;
  static std::shared_ptr<STDIODFTracer> instance;
  std::unordered_map<FILE *, std::string> tracked_fh;
  std::shared_ptr<DFTLogger> logger;
  bool trace_all_files;

  inline const char *is_traced(FILE *fh, const char *func) {
    DFTRACER_LOGDEBUG("Calling STDIODFTracer.is_traced for %s", func);
    if (fh == NULL) return nullptr;
    auto iter = tracked_fh.find(fh);
    if (iter != tracked_fh.end()) {
      return iter->second.c_str();
    }
    return nullptr;
  }

  inline const char *is_traced(const char *filename, const char *func) {
    DFTRACER_LOGDEBUG("Calling STDIODFTracer.is_traced with filename for %s",
                      func);
    if (stop_trace) return nullptr;
    if (trace_all_files)
      return filename;
    else
      return is_traced_common(filename, func);
  }

  inline void trace(FILE *fh, const char *filename) {
    DFTRACER_LOGDEBUG("Calling STDIODFTracer.trace with filename", "");
    tracked_fh.insert_or_assign(fh, filename);
  }

  inline void remove_trace(FILE *fh) {
    DFTRACER_LOGDEBUG("Calling STDIODFTracer.remove_trace with filename", "");
    tracked_fh.erase(fh);
  }

 public:
  STDIODFTracer(bool trace_all)
      : STDIO(), tracked_fh(), trace_all_files(trace_all) {
    DFTRACER_LOGDEBUG("STDIO class intercepted", "");
    logger = DFT_LOGGER_INIT();
  }
  void finalize() {
    DFTRACER_LOGDEBUG("Finalizing STDIODFTracer", "");
    stop_trace = true;
  }
  ~STDIODFTracer() { DFTRACER_LOGDEBUG("Destructing STDIODFTracer", ""); };

  static std::shared_ptr<STDIODFTracer> get_instance(bool trace_all = false) {
    DFTRACER_LOGDEBUG("STDIO class get_instance", "");
    if (!stop_trace && instance == nullptr) {
      instance = std::make_shared<STDIODFTracer>(trace_all);
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
#endif  // DFTRACER_STDIO_H
