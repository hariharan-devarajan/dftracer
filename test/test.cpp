//
// Created by hariharan on 8/8/22.
//

#include <fcntl.h>
#include <cassert>
#include <unistd.h>

int main(int argc, char* argv[]) {
  if (argc != 2) assert(argc != 2);
  char* filename = argv[1];
  int fd = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
  assert(fd != -1);
  int status = close(fd);
  assert(status == 0);
  return 0;
}