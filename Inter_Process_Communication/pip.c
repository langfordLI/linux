#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#define K 1024
#define WRITELEN (128 * K)

int main()
{
	int result = -1;
	int fd[2], nbytes; // file descriptor and num of character
	pid_t pid;
	char string[WRITELEN] = "hello,pipe";
	char readbuffer[10*K];
	/*fd1 used for write, fd0 used for read*/
	int *write_fd = &fd[1];
	int *read_fd = &fd[0];
	/*create pipe*/
	result = pipe(fd);
	if(result == -1)
	{
		printf("pipe create failed\n");
		return -1;
	}

	pid = fork();
	if(pid == -1)
	{
		printf("fork process failed\n");
		return -1;
	}
	if(pid == 0) // if child process, close read
	{
		int write_size = WRITELEN; // length which will write in
		result = 0;
		close(*read_fd);
		while(write_size >= 0) // if no date write continue write
		{
			result = write(*write_fd, string, write_size);
			if(result > 0)
			{
				write_size -= result; // write length
				printf("write %d date, remain %d date\n", result, write_size);
			}
			else
			{
				sleep(10);
			}
		}
		return 0;
	}
	else
	{
		close(*write_fd);
		while(1)
		{
			nbytes = read(*read_fd, readbuffer, sizeof(readbuffer));
			if(nbytes <= 0)
			{
				printf("no data\n");
				break;
			}
			printf("receive %d date: %s\n", nbytes, readbuffer);
		}
		return 0;
	}
	return 0;
}
