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
#define LED_ON  1
#define LEDOFF _IOR(230,2,int)
static void close_device(void);
static void open_device(void);
static void set_bell_freq(int freq);
static void stop_bell(void);
static int fd = -1;
static int fd1 = -1;
static int fd_led = -1;
int main(int argc, char **argv)
{
	//設置蜂鳴器的頻率的初始值為1000
	int duty_cycle = 15 ,i=0,duty_cycle1;
	//打開蜂鳴器
	open_device();
	stop_bell();
	while( 1 )
	{
		int i;
		int val = -1;
		int num[2]={0};
		int led=1,fan=2;
		printf("please select number to control ship\n");
			printf("1:adc   2:exit : 3:Run ->");
			scanf("%d", &val);	
			printf("\n");
		ioctl(fd1,0,&num);
		printf("user space x %d y %d\n",num[0],num[1]);
			switch(val)
				{
				case 1:
					for(i=0;i>10;i++){
					ioctl(fd1,0,&num);
					sleep(1);
					}
					break;
				case 2:
					close_device();
					return 0;
				case 3:
					while(1){
					ioctl(fd1,0,&num);
					if ((num[1]<700)&&(duty_cycle<20)){
						duty_cycle++;
						set_bell_freq(duty_cycle);
					}
					if ((num[1]>2000)&&(duty_cycle>9)){
						duty_cycle--;
						set_bell_freq(duty_cycle);
					}
					if (num[0]<300){
						ioctl(fd_led,LED_ON,&led);
						printf("motor on\n");
					}else{
						ioctl(fd_led,LEDOFF,&led);
						printf("motor off\n");
					}
					
						usleep(1000);
					printf("now duty_cycle=%d\n",duty_cycle);
					}
				default:
					close_device();
					return 0;
					
			}
		//周而復始的執行，於是蜂鳴器就會像唱歌一樣
	}
}

//打開蜂鳴器
static void open_device(void)
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
	fd_led= open("/dev/led", 0);
	
	//如果打開小於0表示失敗
	if (fd1 < 0) {
		perror("open led device");
		exit(1);
	}
	//初始化蜂鳴器的時候先關閉，不讓它叫
	atexit(close_device);
}



//關閉峰鳴器
static void close_device(void)
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
		close(fd_led);
		fd_led = -1;
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

