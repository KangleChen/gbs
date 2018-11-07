#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[]) {
  if (argc <= 1) {
    printf("Parameter erforderlich!");
    return 0;
  }

  time_t now;
  time(&now);
  printf("Start: %s", ctime(&now));

  int returnValue = fork();

  if (returnValue == 0) {
    for (int i = 1; i <= atoi(argv[1]); ++i) {
      sleep(1);
      printf("%d %d %d\n", getpid(), getppid(), i);
    }
    return (getpid() + atoi(argv[1])) % 100;
  } else if (returnValue == -1) {
    printf("Creation of child process is not successful.");
    return -1;
  } else {
    int stat;
    wait(&stat);
    printf("Exit-Code: %d\n", WEXITSTATUS(stat));
  }

  time(&now);
  printf("Ende: %s", ctime(&now));
}