
#include <dlio_profiler/core/constants.h>
#include <dlio_profiler/core/dlio_profiler_main.h>
#include <dlio_profiler/dlio_logger.h>
#include <dlio_profiler/utils/configuration_manager.h>
#include <dlio_profiler/utils/utils.h>
#include <execinfo.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

namespace py = pybind11;
namespace dlio_profiler {

    void initialize(const char *log_file, const char *data_dirs, int process_id) {
      auto conf = dlio_profiler::Singleton<dlio_profiler::ConfigurationManager>::get_instance();
      DLIO_PROFILER_LOGDEBUG("py.initialize","");
      dlio_profiler::Singleton<dlio_profiler::DLIOProfilerCore>::get_instance(ProfilerStage::PROFILER_INIT,
                                                                              ProfileType::PROFILER_PY_APP, log_file,
                                                                              data_dirs, &process_id);
    }

    TimeResolution get_time() {
      DLIO_PROFILER_LOGDEBUG("py.get_time","");
      auto dlio_profiler_inst = dlio_profiler::Singleton<dlio_profiler::DLIOProfilerCore>::get_instance(ProfilerStage::PROFILER_OTHER,
                                                                                   ProfileType::PROFILER_PY_APP);
      if (dlio_profiler_inst != nullptr) return dlio_profiler_inst->get_time();
      else DLIO_PROFILER_LOGDEBUG("py.get_time dlio_profiler not initialized","");
      return 0;
    }

    void log_event(std::string name, std::string cat, TimeResolution start_time, TimeResolution duration,
                   std::unordered_map<std::string, int> &int_args,
                   std::unordered_map<std::string, std::string> &string_args,
                   std::unordered_map<std::string, float> &float_args) {
      DLIO_PROFILER_LOGDEBUG("py.log_event","");
      auto args = std::unordered_map<std::string, std::any>();
      for (auto item:int_args) args.insert_or_assign(item.first, item.second);
      for (auto item:string_args) args.insert_or_assign(item.first, item.second);
      for (auto item:float_args) args.insert_or_assign(item.first, item.second);
      auto dlio_profiler_inst = dlio_profiler::Singleton<dlio_profiler::DLIOProfilerCore>::get_instance(ProfilerStage::PROFILER_OTHER,
                                                                              ProfileType::PROFILER_PY_APP);
      if (dlio_profiler_inst != nullptr) dlio_profiler_inst->log(name.c_str(), cat.c_str(), start_time, duration, &args);
      else DLIO_PROFILER_LOGDEBUG("py.log_event dlio_profiler not initialized","");
    }

    void finalize() {
      DLIO_PROFILER_LOGDEBUG("py.finalize","");
      auto conf = dlio_profiler::Singleton<dlio_profiler::ConfigurationManager>::get_instance();
      //if (conf->init_type == ProfileInitType::PROFILER_INIT_FUNCTION) {
      auto dlio_profiler_inst = dlio_profiler::Singleton<dlio_profiler::DLIOProfilerCore>::get_instance(
              ProfilerStage::PROFILER_FINI,
              ProfileType::PROFILER_PY_APP);
      if (dlio_profiler_inst != nullptr) {
        dlio_profiler_inst->finalize();
      }
      //}
      DLIO_PROFILER_LOGINFO("Finalized Py Binding","");
    }
} // dlio_profiler
PYBIND11_MODULE(dlio_profiler_py, m) {
  m.doc() = "Python module for dlio_logger"; // optional module docstring
  m.def("initialize", &dlio_profiler::initialize, "initialize dlio profiler",
        py::arg("log_file") = nullptr,
        py::arg("data_dirs") = nullptr,
        py::arg("process_id") = -1);
  m.def("get_time", &dlio_profiler::get_time, "get time from profiler");
  m.def("log_event", &dlio_profiler::log_event, "log event with args",
        py::arg("name"), py::arg("cat"), py::arg("start_time"), py::arg("duration"),
        py::arg("int_args") = std::unordered_map<std::string, int>(),
        py::arg("string_args") = std::unordered_map<std::string, std::string>(),
        py::arg("float_args") = std::unordered_map<std::string, float>());
  m.def("finalize", &dlio_profiler::finalize, "finalize dlio profiler");
}

