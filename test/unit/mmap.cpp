

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <vector>
int main(int argc, char* argv[]) {
  const off_t MEM_SIZE = 5L;
  const off_t CHAR_SIZE = 3L;
  off_t current_fs = 0;
  int fd = open("file.dat", O_RDWR | O_CREAT, (mode_t)0666);
  if (fd != -1) {
    off_t current_index = 0;
    printf("current_index %ld\n", current_index);
    int status = lseek(fd, MEM_SIZE, SEEK_SET);
    status = write(fd, "", 1);
    if (status == -1) {
      printf("unable to allocate size %ld (%s)\n", MEM_SIZE, strerror(errno));
      return 0;
    }
    current_fs = current_index + MEM_SIZE;
    printf("current_fs %ld\n", current_fs);
    char* dst;
    if ((dst = (char*)mmap(0, current_fs, PROT_READ | PROT_WRITE, MAP_SHARED,
                           fd, 0)) == (caddr_t)-1) {
      printf("mmap error for output\n");
      return 0;
    }
    auto data = std::vector<char>(CHAR_SIZE, 'a');
    for (int i = 0; i < 10; i++) {
      if (current_index + CHAR_SIZE > current_fs) {
        int status = lseek(fd, current_index + MEM_SIZE, SEEK_SET);
        status = write(fd, "", 1);
        if (status == -1) {
          printf("unable to allocate size %ld (%s)\n", MEM_SIZE,
                 strerror(errno));
          return 0;
        }
        current_fs = current_index + MEM_SIZE;
        printf("increased current_fs %ld\n", current_fs);
      }

      auto write_size = CHAR_SIZE;
      memcpy(dst + current_index, data.data(), write_size);
      msync(dst, write_size, MS_ASYNC);
      printf("%s\n", dst + current_index);
      current_index += write_size;
    }
    munmap(dst, current_index);
  }
  close(fd);
  return 0;
}