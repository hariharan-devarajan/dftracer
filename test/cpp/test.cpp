//
// Created by hariharan on 8/8/22.
//

#include <string>
#include <dlio_profiler/dlio_profiler.h>

void foo() {
  DLIO_PROFILER_CPP_FUNCTION();
  sleep(1);
  {
    DLIO_PROFILER_CPP_REGION(CUSTOM);
    sleep(1);
    DLIO_PROFILER_CPP_REGION_START(CUSTOM_BLOCK);
    sleep(1);
    DLIO_PROFILER_CPP_REGION_END(CUSTOM_BLOCK);
  }
}

int main(int argc, char *argv[]) {
  int init = 0;
  if (argc > 2) {
    if (strcmp(argv[2], "1") == 0) {
      DLIO_PROFILER_CPP_INIT(nullptr, nullptr, nullptr);
      init = 1;
    }
  }
  char filename[1024];
  sprintf(filename, "%s/demofile.txt", argv[1]);
  char filename_link[1024];
  sprintf(filename_link, "%s/demofile_link.txt", argv[1]);
  foo();
  truncate(filename, 0);
  FILE *fh = fopen(filename, "w+");
  if (fh != NULL) {
    fwrite("hello", sizeof("hello"), 1, fh);
    fclose(fh);
  }
  struct stat stat_buf;
  struct stat64 stat_buf64;
  stat(filename, &stat_buf);
  lstat(filename, &stat_buf);
  stat64(filename, &stat_buf64);
  lstat64(filename, &stat_buf64);

  link(filename, filename_link);
  unlink(filename_link);
  symlink(filename, filename_link);
  chmod(filename, S_ISUID);
  chown(filename, 0, 0);
  lchown(filename, 0, 0);
  struct utimbuf utimbuf1;
  utime(filename, &utimbuf1);
  char dir[1024];
  sprintf(dir, "%s", argv[1]);
  int dd = open(dir, O_PATH);
  assert(dd != -1);
  fcntl(dd, F_DUPFD);
  fcntl(dd, F_GETFD);
  fcntl(dd, F_GETOWN_EX);
  int dd2;
  dup2(dd, dd2);
  umask(0);
  mkfifo(filename, 0);
  symlinkat(filename, dd, filename_link);
  fstat(dd, &stat_buf);
  fstat64(dd, &stat_buf64);
  faccessat(dd, "demofile.txt", O_RDONLY, 0);
  linkat(dd, "demofile.txt", dd, "demofile_link2.txt", 0);
  chdir(dir);
  int fd = openat(dd, "demofile.txt", O_RDONLY);
  if (fd != -1) close(fd);
  fd = openat(dd, "demofile2.txt", O_WRONLY | O_CREAT, 777);
  if (fd != -1) close(fd);
  close(dd);
  char filename2[1024];
  sprintf(filename, "%s/demofile2.txt", argv[1]);
  fd = creat64(filename, O_RDWR);
  if (fd != -1) close(fd);
  fd = open(filename, O_RDWR);
  int set_offset = lseek(fd, 1, SEEK_SET);
  char buf[1];
  pread(fd, buf, 1, 1);
  pread64(fd, buf, 1, 1);
  pwrite(fd, buf, 1, 1);
  pwrite64(fd, buf, 1, 1);
  fsync(fd);
  fdatasync(fd);
  readlinkat(fd, filename, buf, 1);
  ftruncate(fd, 0);
  close(fd);
  remove(filename2);
  remove(filename);
  remove(filename_link);
  remove("demofile_link2.txt");
  if (init == 1) {
    DLIO_PROFILER_CPP_FINI();
  }
  return 0;
}