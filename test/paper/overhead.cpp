//
// Created by haridev on 10/21/23.
//

#include <dftracer/core/constants.h>
#include <dftracer/core/logging.h>
#include <fcntl.h>
#include <math.h>
#include <mpi.h>
#include <unistd.h>
#include <util.h>

#include <cassert>
#include <cstdlib>

int main(int argc, char* argv[]) {
  MPI_Init(&argc, &argv);
  init_log();
  int my_rank, comm_size;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
  uint64_t num_operations = 1024;
  uint64_t transfer_size = 4096;
  char filename[4096], filename_primary[4096];
  if (argc < 4) {
    DFTRACER_LOG_ERROR(
        "usage: overhead FILENAME <NUM OPERATIONS> <TRANSFER SIZE>", "");
    exit(1);
  }
  fs::create_directories(argv[2]);
  sprintf(filename, "%s/file_%d-%d.bat", argv[2], my_rank, comm_size);
  num_operations = strtoll(argv[3], NULL, 10);
  transfer_size = strtoll(argv[4], NULL, 10);
  MPI_Barrier(MPI_COMM_WORLD);
  if (my_rank == 0) {
    DFTRACER_LOG_INFO("Running with transfer size %lu  and num ops %lu",
                      transfer_size, num_operations);
  }
  long double file_size = num_operations * transfer_size;
  int ts = ceil(file_size / comm_size);
  if (my_rank == 0) {
    DFTRACER_LOG_INFO("Writing %d per rank for data generation", ts);
  }
  sprintf(filename_primary, "%s/file_0-%d.bat", argv[2], comm_size);
  {
    MPI_File fh_orig;
    int status_orig =
        MPI_File_open(MPI_COMM_WORLD, filename_primary,
                      MPI_MODE_RDWR | MPI_MODE_CREATE, MPI_INFO_NULL, &fh_orig);
    assert(status_orig == 0);
    MPI_Offset base_offset = ts * my_rank;
    assert(base_offset >= 0);
    auto write_data = std::vector<char>(ts, 'w');
    MPI_Status stat_orig;
    auto ret_orig = MPI_File_write_at(fh_orig, base_offset, write_data.data(),
                                      ts, MPI_CHAR, &stat_orig);
    assert(ret_orig == 0);
    int written_bytes;
    MPI_Get_count(&stat_orig, MPI_CHAR, &written_bytes);
    if (written_bytes != ts) {
      DFTRACER_LOG_ERROR("Write was unsuccessful written %d of %d bytes",
                         written_bytes, ts);
    }
    assert(written_bytes == ts);
    status_orig = MPI_File_close(&fh_orig);
  }
  if (my_rank != 0) {
    std::string cmd =
        "cp " + std::string(filename_primary) + " " + std::string(filename);
    int status = system(cmd.c_str());
    assert(status != -1);
  }
  assert(fs::file_size(filename) >= file_size);
  MPI_Barrier(MPI_COMM_WORLD);
  if (my_rank == 0) {
    DFTRACER_LOG_INFO("Created dataset for test", "");
  }
  Timer operation_timer;
  MPI_Barrier(MPI_COMM_WORLD);
  if (my_rank == 0) {
    DFTRACER_LOG_INFO("Starting read", "");
  }
  operation_timer.resumeTime();
  int fd = open(filename, O_RDONLY);
  operation_timer.pauseTime();
  assert(fd != -1);
  char* buf = (char*)malloc(transfer_size);
  for (uint64_t i = 0; i < num_operations; ++i) {
    operation_timer.resumeTime();
    ssize_t read_bytes = read(fd, buf, transfer_size);
    operation_timer.pauseTime();
    assert(read_bytes > 0);
    assert(transfer_size == (uint64_t)read_bytes);
    MPI_Barrier(MPI_COMM_WORLD);
    if (i % 10000 == 0 && my_rank == 0) {
      DFTRACER_LOG_INFO("Completed %d loops", i);
    }
  }
  free(buf);
  operation_timer.resumeTime();
  int status = close(fd);
  operation_timer.pauseTime();
  assert(status == 0);

  MPI_Barrier(MPI_COMM_WORLD);
  if (my_rank == 0) {
    DFTRACER_LOG_INFO("Finishing read", "");
  }
  double elapsed_time = operation_timer.getElapsedTime();
  double total_time;
  MPI_Reduce(&elapsed_time, &total_time, 1, MPI_DOUBLE, MPI_SUM, 0,
             MPI_COMM_WORLD);
  if (my_rank == 0) {
    printf("scale,ops,ts,time\n%d,%lu,%lu,%f\n", comm_size, num_operations,
           transfer_size, total_time);
  }
  if (fs::exists(filename)) fs::remove(filename);
  MPI_Finalize();
  return 0;
}