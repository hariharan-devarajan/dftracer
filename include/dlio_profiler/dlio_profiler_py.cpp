
#include <dlio_profiler/dlio_logger.h>
#include <pybind11/pybind11.h>

namespace py = pybind11;
namespace dlio_profiler {
    void initialize() {
      dlio_profiler::Singleton<DLIOLogger>::get_instance();
    }

    void start(std::string &name, std::string &cat) {
      dlio_profiler::Singleton<DLIOLogger>::get_instance()->start(name, cat);
    }

    void update_int(std::string &key, int value) {
      dlio_profiler::Singleton<DLIOLogger>::get_instance()->update_metadata(key, value);
    }

    void update_str(std::string key, std::string value) {
      dlio_profiler::Singleton<DLIOLogger>::get_instance()->update_metadata(key, value);
    }

    void stop() {
      dlio_profiler::Singleton<DLIOLogger>::get_instance()->stop();
    }

    void finalize() {
      dlio_profiler::Singleton<DLIOLogger>::get_instance()->finalize();
    }
} // dlio_profiler
PYBIND11_MODULE(dlio_profiler_py, m) {
  m.doc() = "Python module for dlio_logger"; // optional module docstring
  m.def("initialize", &dlio_profiler::initialize, "initialize_log initializes the log");
  m.def("start", &dlio_profiler::start, "start log timer", py::arg("name"), py::arg("cat"));
  m.def("update_int", &dlio_profiler::update_int, "updates extra args", py::arg("key"), py::arg("value"));
  m.def("update_str", &dlio_profiler::update_str, "updates extra args", py::arg("key"), py::arg("value"));
  m.def("stop", &dlio_profiler::stop, "stop log timer");
  m.def("finalize", &dlio_profiler::finalize, "finalize log finalizes the log");
}

