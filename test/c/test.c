//
// Created by hariharan on 8/8/22.
//

#include <dftracer/dftracer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int bar();
void foo() {
  DFTRACER_C_FUNCTION_START();
  DFTRACER_C_FUNCTION_UPDATE_INT("key", 0);
  DFTRACER_C_FUNCTION_UPDATE_STR("key", "0");
  usleep(1000);
  DFTRACER_C_REGION_START(CUSTOM);
  DFTRACER_C_REGION_UPDATE_INT(CUSTOM, "key", 0);
  DFTRACER_C_REGION_UPDATE_STR(CUSTOM, "key", "0");
  bar();
  usleep(1000);
  DFTRACER_C_REGION_START(CUSTOM_BLOCK);
  usleep(1000);
  DFTRACER_C_REGION_END(CUSTOM_BLOCK);
  DFTRACER_C_REGION_END(CUSTOM);
  DFTRACER_C_FUNCTION_END();
}

int main(int argc, char *argv[]) {
  int init = 0;
  if (argc > 2) {
    if (strcmp(argv[2], "1") == 0) {
      DFTRACER_C_INIT(NULL, NULL, NULL);
      init = 1;
    }
  }
  DFTRACER_C_METADATA(meta, "key", "value");
  char filename[1024];
  sprintf(filename, "%s/demofile_c.txt", argv[1]);
  foo();
  FILE *fh = fopen(filename, "w+");
  fwrite("hello", sizeof("hello"), 1, fh);
  int pid = getpid();
  int child_pid = fork();  // fork a duplicate process
  printf("child pid %d\n", child_pid);
  int child_ppid = getppid();  // get the child's parent pid

  if (child_ppid == pid) {
    // if the current process is a child of the main process
    char *arr[] = {"ls", "-l", NULL};
    execv("/bin/ls", arr);
    exit(1);
  }
  int status = -1;
  waitpid(child_pid, &status, WEXITED);
  fclose(fh);
  if (init) {
    DFTRACER_C_FINI();
  }
  return 0;
}