//
// Created by hariharan on 8/16/22.
//
#include <cpp-logger/logger.h>
#include <dftracer/brahma/stdio.h>
#include <dftracer/df_logger.h>

static ConstEventType CATEGORY = "STDIO";

std::shared_ptr<brahma::STDIODFTracer> brahma::STDIODFTracer::instance =
    nullptr;
bool brahma::STDIODFTracer::stop_trace = false;

FILE *brahma::STDIODFTracer::fopen64(const char *path, const char *mode) {
  BRAHMA_MAP_OR_FAIL(fopen64);
  DFT_LOGGER_START(path);
  DFT_LOGGER_UPDATE(mode);
  FILE *ret = __real_fopen64(path, mode);
  DFT_LOGGER_END();
  if (trace) this->trace(ret, path);
  return ret;
}

FILE *brahma::STDIODFTracer::fopen(const char *path, const char *mode) {
  BRAHMA_MAP_OR_FAIL(fopen);
  DFT_LOGGER_START(path);
  DFT_LOGGER_UPDATE(mode);
  FILE *ret = __real_fopen(path, mode);
  DFT_LOGGER_END();
  if (trace) this->trace(ret, path);
  return ret;
}

int brahma::STDIODFTracer::fclose(FILE *fp) {
  BRAHMA_MAP_OR_FAIL(fclose);
  DFT_LOGGER_START(fp);
  int ret = __real_fclose(fp);
  DFT_LOGGER_END();
  if (trace) this->remove_trace(fp);
  return ret;
}

size_t brahma::STDIODFTracer::fread(void *ptr, size_t size, size_t nmemb,
                                    FILE *fp) {
  BRAHMA_MAP_OR_FAIL(fread);
  DFT_LOGGER_START(fp);
  DFT_LOGGER_UPDATE(size);
  DFT_LOGGER_UPDATE(nmemb);
  size_t ret = __real_fread(ptr, size, nmemb, fp);
  DFT_LOGGER_END();
  return ret;
}

size_t brahma::STDIODFTracer::fwrite(const void *ptr, size_t size, size_t nmemb,
                                     FILE *fp) {
  BRAHMA_MAP_OR_FAIL(fwrite);
  DFT_LOGGER_START(fp);
  DFT_LOGGER_UPDATE(size);
  DFT_LOGGER_UPDATE(nmemb);
  size_t ret = __real_fwrite(ptr, size, nmemb, fp);
  DFT_LOGGER_END();
  return ret;
}

long brahma::STDIODFTracer::ftell(FILE *fp) {
  BRAHMA_MAP_OR_FAIL(ftell);
  DFT_LOGGER_START(fp);
  long ret = __real_ftell(fp);
  DFT_LOGGER_END();
  return ret;
}

int brahma::STDIODFTracer::fseek(FILE *fp, long offset, int whence) {
  BRAHMA_MAP_OR_FAIL(fseek);
  DFT_LOGGER_START(fp);
  DFT_LOGGER_UPDATE(offset);
  DFT_LOGGER_UPDATE(whence);
  int ret = __real_fseek(fp, offset, whence);
  DFT_LOGGER_END();
  return ret;
}