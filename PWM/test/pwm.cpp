#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#define DEVICE_ADC "/dev/adc" 
//定義蜂鳴器打開的操作
#define PWM_OPEN	1
//定義蜂鳴器關閉的操作
#define PWM_STOP	0
static void close_bell(void);
static void open_bell(void);
static void set_bell_freq(int freq);
static void stop_bell(void);
static int fd = -1;
static int fd1 = -1;
int main(int argc, char **argv)
{
	//設置蜂鳴器的頻率的初始值為1000
	int duty_cycle = 15 ,i=0;
	//打開蜂鳴器
	open_bell();
	stop_bell();
	while( 1 )
	{
		
		int val = -1;
		int num=0;
		printf("please select number to control led\n");
			printf("1:duty   2:exit : ->");
			scanf("%d", &val);	
			printf("\n");
			if(val !=2){
				printf("set duty_cycle");
				scanf("%d", &duty_cycle);
				printf("\n");
				}
		ioctl(fd1,0,&num);
		printf("now adc 0 value %d\n",num);
			switch(val)
				{
				case 1:
					for(i=2;i<25;i++){
						set_bell_freq(i);
						usleep(300000);
					}
					break;
				case 2:
					close_bell();
					return 0;
				case 3:
					while(1){
					ioctl(fd1,0,&num);
					if ((num<700)&&(duty_cycle<25)){
						duty_cycle++;
						set_bell_freq(duty_cycle);
					}
					if ((num>2000)&&(duty_cycle>2)){
						duty_cycle--;
						set_bell_freq(duty_cycle);
					}
					
					
						usleep(30000);
					}
				default:
					if(0<duty_cycle<20)
						set_bell_freq(duty_cycle);
					else
						;
					
			}
		//周而復始的執行，於是蜂鳴器就會像唱歌一樣
	}
}

//打開蜂鳴器
static void open_bell(void)
{
	//打開設備
	fd = open("/dev/s3c24xx-pwm", 0);
	
	//如果打開小於0表示失敗
	if (fd < 0) {
		perror("open pwm_buzzer device");
		exit(1);
	}
	
	fd1 = open(DEVICE_ADC, 0);
	
	//如果打開小於0表示失敗
	if (fd1 < 0) {
		perror("open adc device");
		exit(1);
	}

	//初始化蜂鳴器的時候先關閉，不讓它叫
	atexit(close_bell);
}



//關閉峰鳴器
static void close_bell(void)
{
	if (fd >= 0) {
		//關閉蜂鳴器
		ioctl(fd, PWM_STOP);
		if (ioctl(fd, 2) < 0) {
			perror("ioctl 2:");
		}
		close(fd);
		fd = -1;
		close(fd1);
		fd1 = -1;
	}
}
//設置蜂鳴器的頻率
static void set_bell_freq(int freq)
{
	//設置頻率
	int ret = ioctl(fd, PWM_OPEN, freq);
	if(ret < 0) {
		perror("set the frequency of the buzzer");
		exit(1);
	}
}
//停止蜂鳴器
static void stop_bell(void)
{
	//讓蜂鳴器停止叫
	int ret = ioctl(fd, PWM_STOP);
	if(ret < 0) {
		perror("stop the buzzer");
		exit(1);
	}
	if (ioctl(fd, 2) < 0) {
		perror("ioctl 2:");
	}
}

