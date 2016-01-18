#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/times.h>

#define N_SIZE 5
#define SEG_SIZE 6
#define LOOP 5

int data[50000000];
int n_array[N_SIZE] = {100, 10000, 1000000, 10000000, 50000000};
int seg_array[SEG_SIZE] = {100, 25, 10, 5, 2, 1};
double real[N_SIZE][SEG_SIZE], user[N_SIZE][SEG_SIZE], sys[N_SIZE][SEG_SIZE];

static void pr_times(clock_t realt, struct tms *tmsstart, struct tms *tmsend, int i, int j) {
    long clktck = sysconf(_SC_CLK_TCK);
    real[i][j] += realt / (double) clktck;
    user[i][j] += (tmsend->tms_cutime - tmsstart->tms_cutime) / (double) clktck;
    sys[i][j] += (tmsend->tms_cstime - tmsstart->tms_cstime) / (double) clktck;
}

int main() {
    int fd_null = open("/dev/null", O_WRONLY);
    srand(time(0));
    int fd_pipe[2];
    for (int z=0; z<LOOP; z++) {
        for (int i=0; i<N_SIZE; i++) {
            for (int j=0; j<n_array[i]; j++) data[j] = rand();
            for (int j=0; j<SEG_SIZE; j++) {
                fprintf(stderr, "Processing: %d %d\n", n_array[i], seg_array[j]);
                pipe(fd_pipe);
                int pid;
                struct tms tmsstart, tmsend;
                clock_t start, end;
                start = times(&tmsstart);
                if ((pid = fork()) == 0) {
                    dup2(fd_pipe[0], STDIN_FILENO);
                    dup2(fd_null, STDOUT_FILENO);
                    close(fd_pipe[0]);
                    close(fd_pipe[1]);
                    char tmp[50];
                    sprintf(tmp, "%d", n_array[i] / seg_array[j]);
                    execl("merger", "./merger", tmp, (char*)0);
                }
                FILE *fp = fdopen(fd_pipe[1], "w");
                fprintf(fp, "%d\n", n_array[i]);
                for (int k=0; k<n_array[i]; k++) fprintf(fp, "%d ", data[k]); 
                fprintf(fp, "\n");
                fflush(fp);
                fsync(fd_pipe[1]);
                wait(NULL);
                end = times(&tmsend);
                pr_times(end-start, &tmsstart, &tmsend, i, j);
            }
        }
    }
    printf("real: \n");
    for (int i=0; i<N_SIZE; i++) {
        for (int j=0; j<SEG_SIZE; j++) {
            printf("%7.3lf", real[i][j] / LOOP);
        }
        printf("\n");
    }
    printf("user: \n");
    for (int i=0; i<N_SIZE; i++) {
        for (int j=0; j<SEG_SIZE; j++) {
            printf("%7.3lf", user[i][j] / LOOP);
        }
        printf("\n");
    }
    printf("sys: \n");
    for (int i=0; i<N_SIZE; i++) {
        for (int j=0; j<SEG_SIZE; j++) {
            printf("%7.3lf", sys[i][j] / LOOP);
        }
        printf("\n");
    }
    return 0;
}
