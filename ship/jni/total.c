#include <jni.h>
#include <Android/log.h>
#include <stdio.h>
#include <stdlib.h>	//system
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define  TAG    "meme37"
// 定義info資訊
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
// 定義debug資訊
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
// 定義error資訊
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)

#define PWM_OPEN	1
#define PWM_STOP	0
#define DEVICE_BLTEST "/dev/led"
int fd,pwm_fd,adc_fd;
int server_sockfd,client_sockfd,i=0;
int server_len,client_len;
//int buf[10];
int duty;
struct sockaddr_in server_address;
struct sockaddr_in client_address;
int num[2]={0};
int flag=0;
JNIEXPORT jint JNICALL Java_com_meme37_ship_Linuxc_openled
	(JNIEnv *env, jclass thiz){


    fd=open(DEVICE_BLTEST,O_RDONLY);
  return fd;


}
 JNIEXPORT jint JNICALL Java_com_meme37_ship_Linuxc_closeled
  (JNIEnv *env, jclass thiz){
	 close(pwm_fd);
	 close(adc_fd);
  return close(fd);

}
JNIEXPORT jint JNICALL Java_com_meme37_ship_Linuxc_send
  (JNIEnv *env, jclass thiz ,  jint val, jint num){
  int ret;
  ret= ioctl(fd,num,&val);

  return ret;
}

JNIEXPORT jint JNICALL Java_com_meme37_ship_Linuxc_openpwm
  (JNIEnv *env, jclass thiz){
	pwm_fd = open("/dev/s3c24xx-pwm", 0);
	if (pwm_fd < 0) {
			perror("open pwm device");
			exit(1);
		}
  return pwm_fd;

}
JNIEXPORT void JNICALL Java_com_meme37_ship_Linuxc_stoppwm
  (JNIEnv *env, jclass thiz){
	int ret = ioctl(pwm_fd, PWM_STOP);
		if(ret < 0) {
			perror("stop the pwm");
			exit(1);
		}
		if (ioctl(pwm_fd, 2) < 0) {
			perror("ioctl 2:");
		}

}
JNIEXPORT void JNICALL Java_com_meme37_ship_Linuxc_setpwm
  (JNIEnv *env, jclass thiz,jint duty){
	printf("duty=\n");
	int ret = ioctl(pwm_fd, PWM_OPEN,duty);
		if(ret < 0) {
			perror("set the frequency of the motor");
			exit(1);
		}

}
JNIEXPORT jint JNICALL Java_com_meme37_ship_Linuxc_openadc
  (JNIEnv *env, jclass thiz){
	adc_fd = open("/dev/adc", 0);
	if (adc_fd < 0) {
			perror("open adc device");
			exit(1);
		}
  return adc_fd;

}

JNIEXPORT  JNICALL Java_com_meme37_ship_Linuxc_readadc
  (JNIEnv *env, jclass thiz){
	jintArray num1;
	num1 = (*env)->NewIntArray(env, 2);
	ioctl(adc_fd,0,&num);
	(*env)->SetIntArrayRegion(env, num1,0 , 2, num);



  return  num1;

}


JNIEXPORT jint JNICALL Java_com_meme37_ship_Linuxc_opensocket
	(JNIEnv *env, jclass thiz){
	if((server_sockfd=socket(AF_INET,SOCK_STREAM,0))<0){

			LOGE("%s error\n", "create socket error!\n");
			exit(1);
		}
	server_address.sin_family=AF_INET;
	server_address.sin_addr.s_addr=htonl(INADDR_ANY);
	server_address.sin_port=htons(9734);
	server_len=sizeof(server_address);
	fcntl(server_sockfd,F_SETFL,O_NONBLOCK);
	bind(server_sockfd,(struct sockaddr*)&server_address,server_len);
	listen(server_sockfd,5);
	//signal(SIGTSTP,SIG_IGN);
	return server_sockfd;
}

JNIEXPORT jint JNICALL Java_com_meme37_ship_Linuxc_connectclient
  (JNIEnv *env, jclass thiz){
	client_len=sizeof(client_address);
	client_sockfd=accept(server_sockfd,(struct sockaddr*)&client_address,&client_len);
	if(flag==0){
		LOGI("%s\n", "server waiting.....");
		flag=1;
	}
	if(client_sockfd>0){
		LOGI("a new connect port=%d fd=%d\n",client_address.sin_port,client_sockfd);
		flag=0;
	}
  return client_sockfd;

}

JNIEXPORT jint JNICALL Java_com_meme37_ship_Linuxc_readclient
  (JNIEnv *env, jclass thiz){

	if(read(client_sockfd,&duty,sizeof(duty))>0){
		LOGI("recv=%d\n",duty);
		return duty;
		}

	return 0;


}


JNIEXPORT jint JNICALL Java_com_meme37_ship_Linuxc_closesocket
  (JNIEnv *env, jclass thiz){
	 close(client_sockfd);
  return close(server_sockfd);

}

