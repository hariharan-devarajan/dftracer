//
// Created by hariharan on 8/8/22.
//


#include <string>
int main(int argc, char* argv[]) {
  FILE* fh = fopen("./data/demofile.txt", "w+");
  fwrite("hello", sizeof("hello"), 1, fh);
  fclose(fh);
  return 0;
}