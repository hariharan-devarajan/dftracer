//
// Created by haridev on 8/23/23.
//

#ifndef DLIO_PROFILER_UTILS_H
#define DLIO_PROFILER_UTILS_H

#include <string>
#include <limits.h>
#include <dlio_profiler/utils/posix_internal.h>
#include <vector>
#include <vector>
#include <cstring>
#include <dlio_profiler/core/macro.h>
#include <iostream>
#include <execinfo.h>
#include <sstream>
#include <regex>

inline std::string getexepath() { // GCOVR_EXCL_START
  char result[PATH_MAX];
  ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
  return std::string(result, (count > 0) ? count : 0);
}

inline std::string sh(std::string cmd) {
  std::array<char, 128> buffer;
  std::string result;
  std::shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
  if (!pipe) throw std::runtime_error("popen() failed!");
  while (!feof(pipe.get())) {
    if (fgets(buffer.data(), 128, pipe.get()) != nullptr) {
      result += buffer.data();
    }
  }
  return result;
}

inline void print_backtrace(void) {
  void *bt[1024];
  int bt_size;
  char **bt_syms;
  int i;

  bt_size = backtrace(bt, 1024);
  bt_syms = backtrace_symbols(bt, bt_size);
  std::regex re("\\[(.+)\\]");
  auto exec_path = getexepath();
  std::string addrs = "";
  for (i = 1; i < bt_size; i++) {
    std::string sym = bt_syms[i];
    std::smatch ms;
    if (std::regex_search(sym, ms, re)) {
      std::string m = ms[1];
      addrs += " " + m;
    }
  }
  auto r = sh("addr2line -e " + exec_path + " -f -C " + addrs);
  std::cout << r << std::endl;
  free(bt_syms);
}

inline void signal_handler(int sig) {
  switch (sig) {
    case SIGHUP: {
      DLIO_PROFILER_LOGPRINT("hangup signal caught", 0);
      break;
    }
    case SIGTERM: {
      DLIO_PROFILER_LOGPRINT("terminate signal caught", 0);
      //MPI_Finalize();
      exit(0);
      break;
    }
    default: {
      DLIO_PROFILER_LOGPRINT("signal caught %d", sig);
      //print_backtrace();
      void *array[20];
      size_t size;
      void *trace[16];
      char **messages = (char **) NULL;
      int i, trace_size = 0;

      size = backtrace(array, 20);
      messages = backtrace_symbols(array, size);
      /* skip first stack frame (points here) */
      std::stringstream myString;
      myString << "[bt] Execution path with signal " << sig << ":\n";
      for (i = 1; i < size; ++i) {
        //printf("%s\n", messages[i]);
        std::string m_string(messages[i]);
        //./prog(myfunc3+0x5c) [0x80487f0]
        std::size_t open_paren = m_string.find_first_of("(");
        if (open_paren == std::string::npos) {
          myString << "[bt] #" << i << " " << messages[i] << "\n";
          continue;
        }
        std::size_t plus = m_string.find_first_of("+");
        std::size_t close_paren = m_string.find_first_of(")");
        std::size_t open_square = m_string.find_first_of("[");
        std::size_t close_square = m_string.find_first_of("]");
        std::string prog_name = m_string.substr(0, open_paren);
        std::string func_name = m_string.substr(open_paren + 1, plus - open_paren - 1);
        std::string offset = m_string.substr(plus + 1, close_paren - plus - 1);
        std::string addr = m_string.substr(open_square + 1, close_square - open_square - 1);
        if (func_name.empty()) {
          myString << "[bt] #" << i << " " << messages[i] << "\n";
          continue;
        }
        char command[256];
        sprintf(command, "nm %s | grep \"\\s%s$\" | awk '{print $1}'", prog_name.c_str(), func_name.c_str());
        std::string base_addr = sh(command);
        sprintf(command, "python2 -c \"print hex(0x%s+%s)\"", base_addr.c_str(), offset.c_str());
        std::string hex_val = sh(command);
        sprintf(command, "addr2line -e %s %s", prog_name.c_str(), hex_val.c_str());
        std::string line = sh(command); // line has a new line char already
        myString << "[bt] #" << i << " " << prog_name << "(" << func_name << "+" << offset << ")" << line;
      }
      std::string res = myString.str();
      std::cout << res;
      ::raise(SIGTERM);
    }

  }
}

inline void set_signal() {
  struct sigaction sa;
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  sigaction(SIGSEGV, &sa, NULL);
  sigaction(SIGUSR1, &sa, NULL);
  sigaction(SIGABRT, &sa, NULL);
  sigaction(SIGHUP, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);
}  // GCOVR_EXCL_STOP

const std::string ignore_filenames[5] = {".pfw", "/pipe", "/socket", "/proc/self", ".py"};

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

inline bool ignore_files(const char *filename) {
  for (auto &file: ignore_filenames) {
    if (strstr(filename, file.c_str()) != NULL) {
      return true;
    }
  }
  return false;
}

inline std::string get_filename(int fd) {
  char proclnk[PATH_MAX];
  char filename[PATH_MAX];
  snprintf(proclnk, PATH_MAX, "/proc/self/fd/%d", fd);
  size_t r = dlp_readlink(proclnk, filename, PATH_MAX);
  filename[r] = '\0';
  return filename;
}

inline std::pair<bool, std::string> is_traced_common(const char *filename, const char *func,
                                                     const std::vector<std::string> &ignore_filename,
                                                     const std::vector<std::string> &track_filename) {
  bool found = false;
  bool ignore = false;
  char resolved_path[PATH_MAX];
  char *data = realpath(filename, resolved_path);
  (void) data;
  if (ignore_files(resolved_path) || ignore_files(filename)) {
    DLIO_PROFILER_LOGINFO("Profiler ignoring file %s for func %s", resolved_path, func);
    return std::pair<bool, std::string>(false, filename);
  }
  for (const auto file : ignore_filename) {
    if (strstr(resolved_path, file.c_str()) != NULL) {
      ignore = true;
      break;
    }
  }
  if (!ignore) {
    for (const auto file : track_filename) {
      if (strstr(resolved_path, file.c_str()) != NULL) {
        found = true;
        break;
      }
    }
  }
  if (!found and !ignore)
    DLIO_PROFILER_LOGINFO("Profiler Intercepted POSIX not tracing file %s for func %s", resolved_path, func);
  return std::pair<bool, std::string>(found, filename);
}

#endif // DLIO_PROFILER_UTILS_H
