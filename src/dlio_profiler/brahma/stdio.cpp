//
// Created by hariharan on 8/16/22.
//
#include <cpp-logger/logger.h>
#include <dlio_profiler/brahma/stdio.h>
#include <dlio_profiler/dlio_logger.h>

static ConstEventType CATEGORY = "STDIO";

std::shared_ptr<brahma::STDIODLIOProfiler> brahma::STDIODLIOProfiler::instance = nullptr;
bool brahma::STDIODLIOProfiler::stop_trace = false;

FILE *brahma::STDIODLIOProfiler::fopen64(const char *path, const char *mode) {
  BRAHMA_MAP_OR_FAIL(fopen64);
  DLIO_LOGGER_START(path);
  DLIO_LOGGER_UPDATE(mode);
  FILE *ret = __real_fopen64(path, mode);
  DLIO_LOGGER_END();
  if (trace) this->trace(ret, path);
  return ret;
}

FILE *brahma::STDIODLIOProfiler::fopen(const char *path, const char *mode) {
  BRAHMA_MAP_OR_FAIL(fopen);
  DLIO_LOGGER_START(path);
  DLIO_LOGGER_UPDATE(mode);
  FILE *ret = __real_fopen(path, mode);
  DLIO_LOGGER_END();
  if (trace) this->trace(ret, path);
  return ret;
}

int brahma::STDIODLIOProfiler::fclose(FILE *fp) {
  BRAHMA_MAP_OR_FAIL(fclose);
  DLIO_LOGGER_START(fp);
  int ret = __real_fclose(fp);
  DLIO_LOGGER_END();
  if (trace) this->remove_trace(fp);
  return ret;
}

size_t brahma::STDIODLIOProfiler::fread(void *ptr, size_t size, size_t nmemb,
                                        FILE *fp) {
  BRAHMA_MAP_OR_FAIL(fread);
  DLIO_LOGGER_START(fp);
  DLIO_LOGGER_UPDATE(size);
  DLIO_LOGGER_UPDATE(nmemb);
  size_t ret = __real_fread(ptr, size, nmemb, fp);
  DLIO_LOGGER_END();
  return ret;
}

size_t brahma::STDIODLIOProfiler::fwrite(const void *ptr, size_t size, size_t nmemb,
                                         FILE *fp) {
  BRAHMA_MAP_OR_FAIL(fwrite);
  DLIO_LOGGER_START(fp);
  DLIO_LOGGER_UPDATE(size);
  DLIO_LOGGER_UPDATE(nmemb);
  size_t ret = __real_fwrite(ptr, size, nmemb, fp);
  DLIO_LOGGER_END();
  return ret;
}

long brahma::STDIODLIOProfiler::ftell(FILE *fp) {
  BRAHMA_MAP_OR_FAIL(ftell);
  DLIO_LOGGER_START(fp);
  long ret = __real_ftell(fp);
  DLIO_LOGGER_END();
  return ret;
}

int brahma::STDIODLIOProfiler::fseek(FILE *fp, long offset, int whence) {
  BRAHMA_MAP_OR_FAIL(fseek);
  DLIO_LOGGER_START(fp);
  DLIO_LOGGER_UPDATE(offset);
  DLIO_LOGGER_UPDATE(whence);
  int ret = __real_fseek(fp, offset, whence);
  DLIO_LOGGER_END();
  return ret;
}