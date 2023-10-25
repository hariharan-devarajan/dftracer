//
// Created by haridev on 10/24/23.
//
#include <uv.h>
void do_work(uv_work_t *req) {
    int n = *(int *) req->data;
    uv_sleep(1000);
    fprintf(stderr, "Running work %d\n", n);
}

void after_work(uv_work_t *req, int status) {
    fprintf(stderr, "Done work %d with status %d\n", *(int *) req->data, status);
}

int main(int argc, char* argv[]) {
  uv_loop_t* loop = uv_default_loop();
  int i;
  const int FIB_UNTIL = 10;
  int data[FIB_UNTIL];
  uv_work_t req[FIB_UNTIL];
  for (i = 0; i < FIB_UNTIL; i++) {
    data[i] = i;
    req[i].data = (void *) &data[i];
    uv_queue_work(loop, &req[i], do_work, after_work);
  }
  fprintf(stderr, "Waiting to finish work\n");
  uv_run(loop, UV_RUN_DEFAULT);
  fprintf(stderr, "finished work\n");
  return 0;
}