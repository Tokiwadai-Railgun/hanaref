#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>


int check_for_env() {
	struct stat buffer;
	int dot_env = open(".env", O_RDONLY);
	fstat(dot_env, &buffer);

	printf("Size is : %ld", buffer.st_size);
	return 0;
}


int main(int argc, char** args) {
	printf("Hello world\n");
}
