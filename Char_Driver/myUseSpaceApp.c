#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

char data[1024];
char data1[1024];

int main() {
	int fd, option = 0;

	printf("Char Driver demo\n");

	fd = open("/dev/my_device", O_RDWR);
	if(fd < 0) {
		perror("Error Occur: ");
		return 0;
	}
	
	while(1) {
		printf("Enter the option to select\n1. Write\n2. Read \n3 exit\n");
		scanf("%d", &option);
		printf("Option selected is: %d\n", option);
		switch(option) {
			case 1:
				printf("Enter the string to send: \n");
				memset(data, '\0', sizeof(data));
				//gets(data);
				scanf("%s", data);
				write(fd, data, strlen(data)+1);
				printf("data written successfully\n");
				break;
			case 2:
				printf("Reading data\n");
				read(fd, data1, sizeof(data1));
				printf("data is: %s\n", data1);
				break;
			case 3:
				close(fd);
				printf("exiting the code\n");
				exit(0);
			default:
				printf("Invalid option, Enter Valid one\n");
				break;
		}
	}
	close(fd);
}
