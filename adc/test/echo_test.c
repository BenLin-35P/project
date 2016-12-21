
#include <stdio.h>
#include <stdlib.h>	//system
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#define DEVICE_BLTEST "/dev/adc" 
#define LEDOFF _IOR(233,2,int)
#define ECHO _IOR(233,3,int)

#define ECHO_ON  1
#define ECHO_OFF 2
#define ECHO_TEST 3
#define adc _IOR(233,3,int)
     //设备节点

int main(int argc, char *argv[])
{
		int fd;
		int val = -1;
		int num = 1;
		
		fd= open(DEVICE_BLTEST,O_RDWR); //打开设备
		if(fd<0)
		{
			perror("can not open device");
			exit(1);
		}
	
		while(1)
		{
			printf("please select number to control led\n");
			printf("1:led:D608 2:led2 :D609  3:exit : ->");
			scanf("%d", &val);	
			printf("\n");
			if(val !=3){
				printf("select led on or off: 1:ON  2:OFF: ->");
				scanf("%d", &num);
				printf("\n");
				}
			switch(val)
				{
				 case 1: 
				 	if(num == ECHO_ON)
						ioctl(fd,ECHO_ON,&val);
					else if(num == ECHO_OFF)
						ioctl(fd,LEDOFF,&val);

					break;
				 case 2:
				 	if(num == ECHO_ON)
						ioctl(fd,ECHO_ON,&val);
					else if(num == ECHO_OFF)
						ioctl(fd,0,&val);
					break;

				 case 3:
					close(fd);
					return 0;
				 case 4:
					ioctl(fd,ECHO,&val);
					break;
				default:
					return 0;

			}

		}
		//close(fd);
		
		return 0;
}
