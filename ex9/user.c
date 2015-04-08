#include <stdio.h>

int main()
{
	int fd;
	fd=open("file","w");
	getchar();
	close(fd);
	return 0;
}
