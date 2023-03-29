//
// Created by haridev on 3/28/23.
//

#ifndef DLIO_PROFILER_CHROME_WRITER_H
#define DLIO_PROFILER_CHROME_WRITER_H
#include <dlio_profiler/writer/base_writer.h>
#include <string>
#include <unordered_map>
#include <thread>
#include <mutex>
namespace dlio_profiler {
    class ChromeWriter: public BaseWriter {
    private:
        FILE* fp;
        std::string convert_json(std::string &event_name, std::string &category, double &start_time, double &duration,
                                 std::unordered_map<char *, std::any> &metadata);
        size_t tid;
        bool is_first_write;
        std::mutex file_mtx;
    public:
        ChromeWriter(FILE* fp=NULL):BaseWriter(), is_first_write(true){
          this->fp = fp;
          this->tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
        }
        void initialize(char *filename, bool throw_error) override;

        void log(std::string &event_name, std::string &category, double &start_time, double &duration,
                 std::unordered_map<char *, std::any> &metadata) override;

        void finalize() override;
    };
}

#endif //DLIO_PROFILER_CHROME_WRITER_H
