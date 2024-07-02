#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define WR_DATA _IOW('a', 'a', int32_t*)
#define RD_DATA _IOR('a', 'b', int32_t*)

int main() {
	int fd = 0, iNum = 0, iOption = 0;
	int32_t i32Val = 0;

	printf("IOCTL Driver demo\n");

	fd = open("/dev/my_device", O_RDWR);
	if(fd < 0) {
		perror("Error Occur: ");
		return 0;
	}
	
	while(1) {
		printf("Enter the iOption to select\n1. Write\n2. Read \n3 exit\n");
		scanf("%d", &iOption);
		printf("iOption selected is: %d\n", iOption);
		switch(iOption) {
			case 1:
				printf("Enter the num data to send: \n");
				scanf("%d", &i32Val);
				ioctl(fd, WR_DATA, (int32_t *)&i32Val);
				printf("data written successfully\n");
				break;
			case 2:
				printf("Reading data\n");
				ioctl(fd, RD_DATA, (int32_t *)&i32Val);
				printf("data is: %d\n", i32Val);
				break;
			case 3:
				close(fd);
				printf("exiting the code\n");
				exit(0);
			default:
				printf("Invalid iOption, Enter Valid one\n");
				break;
		}
	}
	close(fd);
}
