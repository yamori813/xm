#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/stat.h>

#define SOH		(0x01)
#define STX		(0x02)
#define EOT		(0x04)
#define ACK		(0x06)
#define NAK		(0x15)
#define CAN		(0x18)
#define CTRLZ		(0x1a)

int mksum(unsigned char *buf)
{
int i, sum;

	sum = 0;
	for (i = 0;i < 128; ++i) {
		sum += *buf++;
	}
	sum = sum & 0xff;

	return sum;
}

int main(int argc, char *argv[]) 
{
int fd, kq, ret;
unsigned char buf[256];
int len;
int blocks;
int up;
struct	kevent event;	 /* Event we want to monitor */
struct	kevent tevent;	 /* Event triggered */
struct stat statBuf;
int filelen;
char *filename;
int i;

	if (argc == 2) {
		fd = STDIN_FILENO;
		filename = argv[1];
	} else if (argc == 3) {
		fd = open(argv[1], O_RDWR);
		filename = argv[2];
	} else {
		return -1;
	}

	up = open(filename, O_RDONLY);
	stat(filename, &statBuf);
	filelen = statBuf.st_size;

	kq = kqueue();
	EV_SET(&event,	fd, EVFILT_READ, EV_ADD | EV_CLEAR, NOTE_WRITE,
	    0,	NULL);
	ret = kevent(kq, &event, 1, NULL, 0, NULL);
	if (ret == -1) {
		printf("error kevent register\n");
		return(0);
	}
	if (event.flags & EV_ERROR) {
		printf("error Event\n");
		return(0);
	}
	blocks = 1;
	while(1) {
		ret = kevent(kq, NULL, 0, &tevent, 1, NULL);
		if (ret > 0) {
			len = read(fd, buf, 1);
			if ((buf[0] == NAK && blocks == 1) ||
			    (buf[0] == ACK && filelen > 0)) {
				buf[0] = SOH;
				buf[1] = blocks;
				buf[2] = ~blocks & 0xff;
				write(fd, buf, 1);
				usleep(1000);
				write(fd, buf+1, 1);
				usleep(1000);
				write(fd, buf+2, 1);
				usleep(1000);
				len = read(up, buf, 128);
				if (len != 128)
					memset(buf + len, CTRLZ, 128 - len);
				for (i = 0; i < 128; ++i) {
					write(fd, buf+i, 1);
					usleep(1000);
				}
				buf[0] = mksum(buf);
				write(fd, buf, 1);
				usleep(1000);
				++blocks;
				filelen -= len;
				fprintf(stderr, ".");
				fflush(stderr);
			} else if (buf[0] == ACK && filelen == 0) {
				buf[0] = EOT;
				write(fd, buf, 1);
				filelen = -1;
			} else {
				fprintf(stderr, "\n");
				break;
			}
		}
	}

	return 0;
}
