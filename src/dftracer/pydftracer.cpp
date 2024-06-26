
#include <dftracer/core/constants.h>
#include <dftracer/core/dftracer_main.h>
#include <dftracer/df_logger.h>
#include <dftracer/utils/configuration_manager.h>
#include <dftracer/utils/utils.h>
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
namespace dftracer {

void initialize(const char *log_file, const char *data_dirs, int process_id) {
  auto conf =
      dftracer::Singleton<dftracer::ConfigurationManager>::get_instance();
  DFTRACER_LOGDEBUG("py.initialize", "");
  dftracer::Singleton<dftracer::DFTracerCore>::get_instance(
      ProfilerStage::PROFILER_INIT, ProfileType::PROFILER_PY_APP, log_file,
      data_dirs, &process_id);
}

TimeResolution get_time() {
  DFTRACER_LOGDEBUG("py.get_time", "");
  auto dftracer_inst =
      dftracer::Singleton<dftracer::DFTracerCore>::get_instance(
          ProfilerStage::PROFILER_OTHER, ProfileType::PROFILER_PY_APP);
  if (dftracer_inst != nullptr)
    return dftracer_inst->get_time();
  else
    DFTRACER_LOGDEBUG("py.get_time dftracer not initialized", "");
  return 0;
}

void log_event(std::string name, std::string cat, TimeResolution start_time,
               TimeResolution duration,
               std::unordered_map<std::string, int> &int_args,
               std::unordered_map<std::string, std::string> &string_args,
               std::unordered_map<std::string, float> &float_args) {
  DFTRACER_LOGDEBUG("py.log_event", "");
  auto args = std::unordered_map<std::string, std::any>();
  for (auto item : int_args) args.insert_or_assign(item.first, item.second);
  for (auto item : string_args) args.insert_or_assign(item.first, item.second);
  for (auto item : float_args) args.insert_or_assign(item.first, item.second);
  auto dftracer_inst =
      dftracer::Singleton<dftracer::DFTracerCore>::get_instance(
          ProfilerStage::PROFILER_OTHER, ProfileType::PROFILER_PY_APP);
  if (dftracer_inst != nullptr)
    dftracer_inst->log(name.c_str(), cat.c_str(), start_time, duration, &args);
  else
    DFTRACER_LOGDEBUG("py.log_event dftracer not initialized", "");
}

void enter_event() {
  DFTRACER_LOGDEBUG("py.enter_event", "");
  auto dftracer_inst =
      dftracer::Singleton<dftracer::DFTracerCore>::get_instance(
          ProfilerStage::PROFILER_OTHER, ProfileType::PROFILER_PY_APP);
  dftracer_inst->enter_event();
}

void exit_event() {
  DFTRACER_LOGDEBUG("py.exit_event", "");
  auto dftracer_inst =
      dftracer::Singleton<dftracer::DFTracerCore>::get_instance(
          ProfilerStage::PROFILER_OTHER, ProfileType::PROFILER_PY_APP);
  dftracer_inst->exit_event();
}

void finalize() {
  DFTRACER_LOGDEBUG("py.finalize", "");
  auto conf =
      dftracer::Singleton<dftracer::ConfigurationManager>::get_instance();
  // if (conf->init_type == ProfileInitType::PROFILER_INIT_FUNCTION) {
  auto dftracer_inst =
      dftracer::Singleton<dftracer::DFTracerCore>::get_instance(
          ProfilerStage::PROFILER_FINI, ProfileType::PROFILER_PY_APP);
  if (dftracer_inst != nullptr) {
    dftracer_inst->finalize();
  }
  //}
  DFTRACER_LOGINFO("Finalized Py Binding", "");
}
}  // namespace dftracer
PYBIND11_MODULE(pydftracer, m) {
  m.doc() = "Python module for dftracer";  // optional module docstring
  m.def("initialize", &dftracer::initialize, "initialize dftracer",
        py::arg("log_file") = nullptr, py::arg("data_dirs") = nullptr,
        py::arg("process_id") = -1);
  m.def("get_time", &dftracer::get_time, "get time from profiler");
  m.def("enter_event", &dftracer::enter_event, "mark enter event");
  m.def("exit_event", &dftracer::exit_event, "mark exit event");
  m.def("log_event", &dftracer::log_event, "log event with args",
        py::arg("name"), py::arg("cat"), py::arg("start_time"),
        py::arg("duration"),
        py::arg("int_args") = std::unordered_map<std::string, int>(),
        py::arg("string_args") = std::unordered_map<std::string, std::string>(),
        py::arg("float_args") = std::unordered_map<std::string, float>());
  m.def("finalize", &dftracer::finalize, "finalize dftracer");
}
