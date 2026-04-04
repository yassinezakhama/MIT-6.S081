#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void filter_and_fork(int p[2]);

int main() {
  int p[2];
  pipe(p);

  if (fork() == 0) {
    filter_and_fork(p);
  } else {
    close(p[0]);
    
    for (int n = 2; n <= 35; ++n) {
      write(p[1], &n, sizeof(n));
    }
    
    close(p[1]);
    wait(0);
  }
  
  exit(0);
}

void filter_and_fork(int p[2]) {
  close(p[1]);

  int prime;
  if (read(p[0], &prime, sizeof(prime)) != sizeof(prime)) {
    close(p[0]);
    exit(1);
  }

  printf("prime %d\n", prime);
  int next_pipe[2];
  pipe(next_pipe);

  if (fork() == 0) {
    filter_and_fork(next_pipe);
    exit(0);
  }

  close(next_pipe[0]);
  int next_n; 
  while (read(p[0], &next_n, sizeof(next_n)) == sizeof(next_n)) {
    if (next_n % prime != 0) {
      write(next_pipe[1], &next_n, sizeof(next_n));
    }
  }
  close(next_pipe[1]);
  wait(0);
  exit(0);
}
