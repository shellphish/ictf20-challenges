#include <stdio.h>
#include <fcntl.h>

int main()
{
	open(".", O_RDONLY);
	puts("Hello World!");
}
