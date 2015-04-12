#include <stdio.h>
#include <stdlib.h>

int main()
{
	int fd;
	char buf[50];
	fd = open("new","r");
	read(fd,buf,20);
	getchar();
	getchar();
	close(fd);
	return 0;
}
