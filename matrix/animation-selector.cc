#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>

#define NAME "animation-memory"
#define SIZE 4

int main(int argc, char *argv[]) {
    int shm_fd;
    int *mem;

    if (argc != 2) {
	printf("Error incorrect usage\n");
	return EXIT_FAILURE;
    }

    // open the shared memory object
    shm_fd = shm_open(NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error opening shared memory");
        return EXIT_FAILURE;
    }

    /* Memory map the shared memory object */
    mem = (int*)mmap(0, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (mem == MAP_FAILED) {
        perror("Error mapping shared memory");
        return EXIT_FAILURE;
    }
    
    printf("success mapping memory\n");

    *mem = atoi(argv[1]);

    printf("done\n");

    munmap(mem, SIZE);
    close(shm_fd);

    return 0;
}
