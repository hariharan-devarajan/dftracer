//
// Created by haridev on 3/29/23.
//

#ifndef DLIO_PROFILER_COMMON_H
#define DLIO_PROFILER_COMMON_H
#include <vector>
#include <string>
#include <cstring>
const std::string ignore_filenames[4] = {".pfw", "/pipe", "/socket","/proc/self"};
inline std::vector<std::string> split(std::string str, char delimiter) {
  std::vector<std::string> res;
  if (str.find(delimiter) == std::string::npos) {
    res.push_back(str);
  } else {
    size_t first;
    size_t last = 0;
    while ((first = str.find_first_not_of(delimiter, last)) != std::string::npos) {
      last = str.find(delimiter, first);
      res.push_back(str.substr(first, last - first));
    }
  }
  return res;
}
inline bool ignore_files(const char* filename) {
  for(auto &file: ignore_filenames) {
    if (strstr(filename, file.c_str()) != NULL) {
      return true;
    }
  }
  return false;
}
typedef double TimeResolution;
#endif //DLIO_PROFILER_COMMON_H
