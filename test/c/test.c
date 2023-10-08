//
// Created by hariharan on 8/8/22.
//

#include <dlio_profiler/dlio_profiler.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
void foo() {
  DLIO_PROFILER_C_FUNCTION_START();
  sleep(1);
  DLIO_PROFILER_C_REGION_START(CUSTOM);
  sleep(1);
  DLIO_PROFILER_C_REGION_START(CUSTOM_BLOCK);
  sleep(1);
  DLIO_PROFILER_C_REGION_END(CUSTOM_BLOCK);
  DLIO_PROFILER_C_REGION_END(CUSTOM);
  DLIO_PROFILER_C_FUNCTION_END();
}

int main(int argc, char* argv[]) {
  int init = 0;
  if (argc > 2) {
    if (strcmp(argv[2], "1") == 0) {
      DLIO_PROFILER_C_INIT(NULL,NULL,NULL);
      init = 1;
    }
  }
  char filename[1024];
  sprintf(filename, "%s/demofile.txt", argv[1]);
  foo();
  FILE* fh = fopen(filename, "w+");
  fwrite("hello", sizeof("hello"), 1, fh);
  fclose(fh);
  if(init) {
    DLIO_PROFILER_C_FINI();
  }
  return 0;
}