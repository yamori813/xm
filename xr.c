#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define SOH		(0x01)
#define STX		(0x02)
#define EOT		(0x04)
#define ACK		(0x06)
#define NAK		(0x15)
#define CAN		(0x18)
#define CTRLZ		(0x1a)

int main(int argc, char *argv[]) 
{
int fd, down;
char buf[1];
int len;
int count;
int blocks;

	fd = open(argv[1], O_RDWR);
	down = open(argv[2], O_CREAT|O_WRONLY|O_TRUNC, 0644);
	buf[0] = NAK;
	write(fd, buf, 1);
	blocks = 0;
	count = 0;
	while(1) {
		while(1) {
			len = read(fd, buf, 1);
			if (count == 0 && buf[0] == EOT) {
				buf[0] = ACK;
				write(fd, buf, 1);
				printf("\nfile size %d\n", blocks * 128);
				close(fd);
				close(down);
				return(0);
			}
			if (count > 2 && count < 131) {
				write(down, buf, 1);
			}
			++count;
			if(count == 128 + 4)
				break;
		}
		printf(".");
		fflush(stdout);
		buf[0] = ACK;
		write(fd, buf, 1);
		count = 0;
		++blocks;
	}

	return 0;
}
