#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    char shm_name[100] = {0};
    sprintf(shm_name,"/letter%d",getpid());
    int fd = shm_open(shm_name,O_CREAT | O_RDWR,0644);
    if (fd < 0)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        /* code */
    }
    
    
    return 0;
}
