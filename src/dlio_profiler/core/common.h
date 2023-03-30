//
// Created by haridev on 3/29/23.
//

#ifndef DLIO_PROFILER_COMMON_H
#define DLIO_PROFILER_COMMON_H
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
#endif //DLIO_PROFILER_COMMON_H
