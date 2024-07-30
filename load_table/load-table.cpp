#include "stdint.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <vector>
#include <thread>
#include <cstdio>
#include <cstdlib>


int main(int argc, char** argv)
{
    if (argc != 5) {
    	printf("Not enough arguments got %d\n", argc);
	exit(-1);
    }

    std::vector<uint32_t> data;
    data.push_back(atoi(argv[1]));
    data.push_back(atoi(argv[2]));
    data.push_back(atoi(argv[3]));

    int datfd = open(argv[4], O_RDONLY);
    if (datfd < 0) {
	    perror("opening");
	    exit(-1);
    }

    while (1) {
	    uint32_t c;
	    int ret = read(datfd, &c, 4);
	    if (ret == 0) {
		    break;
	    } else if (ret != 4) {
		    perror("reading");
		    exit(-1);
	    }
		    
	    data.push_back(c);
    }

    int fd = open("/proc/sleep_sched", O_RDONLY);

    if (fd < 0) {
        perror("Could not open sleepy sched control file\n");
        exit(-1);
    }

    printf("%x %p\n", (unsigned int) (data.size() * 4), &data[0]);
    int ret = ioctl(fd, data.size() * 4, &data[0]);

    if (ret != 0) {
        perror("IOCTL failed");
	printf("Error code: %d\n", ret);
        exit(-1);
    }
}
