#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#define FILELENGTH 80

int main()
{
	int fd = -1;
	char buf[] = "quick browm fox jumps over the lazy dog";
	char *ptr = NULL;

	/*open mmap.txt and set 0 for file size*/
	fd = open("mmap.txt", O_RDWR/*can be read*/ | O_CREAT/*if not exist creat*/ | O_TRUNC/*clear file*/, S_IRWXU);
	if(fd == -1)
	{
		return -1;
	}

	lseek(fd, FILELENGTH - 1, SEEK_SET);
	write(fd, "a", 1); // random write a letter for file size is 80

	/*map the file to 1M memory and operate the file, can be read and writen*/
	ptr = (char*)mmap(NULL, FILELENGTH, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if((char*)-1 == ptr)
	{
		printf("mmap failure!\n");
		close(fd);
		return -1;
	}
	memcpy(ptr + 16, buf, strlen(buf));
	munmap(ptr, FILELENGTH);
	close(fd);
	return 0;
}
