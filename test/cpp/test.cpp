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

int main(int argc, char* argv[]) {
  int init = 0;
  if (argc > 2) {
    if (strcmp(argv[2], "1") == 0) {
      DLIO_PROFILER_CPP_INIT(nullptr,nullptr,nullptr);
      init = 1;
    }
  }
  char filename[1024];
  sprintf(filename, "%s/demofile.txt", argv[1]);
  foo();
  FILE* fh = fopen(filename, "w+");
  fwrite("hello", sizeof("hello"), 1, fh);
  fclose(fh);
  if (init ==  1) {
    DLIO_PROFILER_CPP_FINI();
  }
  return 0;
}