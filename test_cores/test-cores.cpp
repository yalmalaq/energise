#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sched.h>
#include <string.h>
#include <sys/syscall.h>
#include <dirent.h>


int klog;

void scan_proc(char *tid)
{
    char buf[500];
    sprintf(buf, "/proc/%s/comm", tid);
    
    FILE *status_file = fopen(buf, "r");
    fgets(buf, sizeof(buf), status_file);
    fclose(status_file);

    if (strstr(buf, "kworker") && strstr(buf, "events\n")) {
        dprintf(klog, "%s %s", tid, buf);

        sprintf(buf, "/proc/%s/stat", tid);
        status_file = fopen(buf, "r");
        while (fgets(buf, sizeof(buf), status_file)) {
            dprintf(klog, "%s", buf);
        }
        fclose(status_file);
        
        sprintf(buf, "/proc/%s/stack", tid);
        status_file = fopen(buf, "r");
        while (fgets(buf, sizeof(buf), status_file)) {
            dprintf(klog, "%s", buf);
        }
        fclose(status_file);
        
    }
}


void scan_procs()
{
    struct dirent *dirent;
    DIR *dir = opendir("/proc");
    
    while ((dirent = readdir(dir))) {
        int pid;
        if (sscanf(dirent->d_name, "%d", &pid) == 1)
            scan_proc(dirent->d_name);
    }
    
    closedir(dir);
}


int main(int argc, char **argv)
{
    klog = open("/dev/kmsg", O_RDWR);
    while (1) {
        scan_procs();
        sleep(3);
    }
}





void print_to(char *path, int to)
{
    FILE *ps = fopen(path, "r");
    if (!ps)
        return;

    char line[500] = {0};
    while (fgets(line, sizeof(line), ps)) {
        write(to, line, strlen(line));
    }

    fclose(ps);
}

int task_pid = -1;

void *print_thread(void *)
{
    int fd = open("/dev/kmsg", O_RDWR);
    while (1) {
        if (task_pid != -1) {
            char path[500];
            sprintf(path, "/proc/%d/stack", task_pid);
            print_to(path, fd);
            sprintf(path, "/proc/%d/status", task_pid);
            print_to(path, fd);
        }
        sleep(1);
    }
    close(fd);
}

int main3(int argc, char** argv)
{
    pthread_t thread;
    pthread_create(&thread, NULL, print_thread, NULL);

    while (1) {
        scanf("%d", &task_pid);
    }

    
    FILE *ps = popen("dmesg -w", "r");
    char line[500] = {0};
    while (fgets(line, sizeof(line), ps)) {
        if (strstr(line, "Sleepy")) {
            char *ptr = strstr(line, "T") + 1;
            task_pid = atoi(ptr);
        }
    }
    fclose(ps);
}

                     



struct thread_args
{
    int coren;
}; 


int print_counter = 0;

void *work_thread(void *in)
{
    struct thread_args *args    = (struct thread_args*) in;
    int fd = open("/dev/kmsg", O_RDWR);


    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(args->coren, &cpuset);


    int i = 0;
    while (1) {
        char msg[50] = {0};

        int affinity = sched_setaffinity(gettid(), sizeof(cpu_set_t), &cpuset);
        unsigned int cpu = 111;
        syscall(__NR_getcpu, &cpu, NULL, NULL);
        
        int msg_len = sprintf(msg, "Hello from core %d (SETAF ret = %d, CORE = %d)\n", args->coren, affinity, cpu);
        write(fd, &msg, msg_len);

        int val = __atomic_fetch_add(&print_counter, 1, __ATOMIC_SEQ_CST);
        if (val % 8 == 0) {
            FILE *ps = popen("ps -o PID,TID,CMDLINE,TIME,SCH,STAT,%CPU,%MEM,ADDR,C,CPU,PSR,NI -A", "r");

            char line[500] = {0};
            while (fgets(line, sizeof(line), ps)) {
                write(fd, line, strlen(line));
            }

            fclose(ps);
        }
        sleep(5);
    }
}

int main2(int argc, char** argv)
{
    setvbuf(stdout, NULL, _IOLBF, 512);

    pthread_t thread;
    for (int i = 0; i < 8; i++) {
        struct thread_args *args = (struct thread_args*) malloc(sizeof(struct thread_args));
        args->coren = i;
        pthread_create(&thread, NULL, work_thread, args);
    }

    while (1) {
        sleep(1000);
    }
}
