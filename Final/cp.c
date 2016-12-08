#include "ucode.c"

int main(int argc, char *argv[])
{
	int src, dest;
	char buffer[BUF_SIZE];
	int i;

	if (argc < 3)
	{
		printf("Usage: cp src dest\n");
		return 0;
	}

	printf("*********** Faustino's Cp ***********\n");
	src = open(argv[1], O_RDONLY);
	dest = open(argv[2], O_WRONLY | O_CREAT);

	
	while((i = read(src, buffer, BUF_SIZE)) > 0)
	{
		write(dest, buffer, i);
	}
	printf("done!\n");

	// Close the src and dest files
	close(src);
	close(dest);
	return 0; 
}