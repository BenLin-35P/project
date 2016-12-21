/*-------------------------------------------------------------------------
 * Filename:      gpio_drv.c
 * Copyright:     Copyright (C) 2006, dmatek
 * Author:        chris_zhang <www.dmatek.com>
 * Description:   gpio driver
 * Created at:     Aug 11 2006
 *-----------------------------------------------------------------------*/
#include <linux/delay.h> 
#include <linux/types.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/poll.h>
#include <linux/string.h>
#include <linux/ioport.h>
#include <asm/uaccess.h>
#include <mach/hardware.h>
#include <asm/io.h>
#include <linux/miscdevice.h>
#include <asm/delay.h>
#include <mach/regs-mem.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <mach/gpio.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <plat/gpio-cfg.h>
#include <plat/gpio-cfg.h>
#include <mach/gpio.h>

#include <linux/pwm.h> //include PWM

#define NS_IN_1HZ				(1000000000UL)//UL =unsigned long

#define PWM_IOCTL_SET_FREQ		1
#define PWM_IOCTL_STOP			0

#define PWM _IOR(231,3,int)
#define PWM_MAJOR 232
#define pwm_name "pwm"

#define EXYNOS4_GPA1CON			(EXYNOS5_GPA1(1) )
#define EXYNOS4_GPA1DAT			(EXYNOS5_GPA1(1) + 0x4)

#define ECHOPIN						EXYNOS4_GPX0(6)
#define TRIGPIN						EXYNOS4_GPX0(0)
static void pwm_set_freq(unsigned long freq) {
//PWM的工作週期的配置
	int period_ns = NS_IN_1HZ / freq;

	pwm_config(pwm4buzzer, period_ns / 2, period_ns); 
	pwm_enable(pwm4buzzer);
	//配置相?的GPIO,?蜂?器IO配置成PWM?出模式
	//s3c_gpio_cfgpin(BUZZER_PMW_GPIO, S3C_GPIO_SFN(2));
}



static ssize_t pwm_read(struct file *filp,char *buf,size_t count,loff_t *l)
{
	return count;
}

static ssize_t pwm_write(struct file *filp,const char *buf,size_t count,loff_t *f_ops)
{
	return count;
}

static long pwm_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	int num;

	printk("cmd=%d\n",cmd);
	switch(cmd)
	{
	
	case 0:
		printk("offff\n");
		ret = copy_from_user(&num, (int*)arg, sizeof(int));
		if (ret != 0)
		{
			printk("gpio_ioctl: copy_from_user failed\n");
			return(-EFAULT);
		}
		printk("num = %d\n",num);
	
		break;	
	case 1:
		printk("onnnnn\n");
		ret = copy_from_user(&num, (int *)arg, sizeof(int));//arg = 3-->num = 3
		if (ret != 0)
		{
			printk("gpio_ioctl: copy_from_user failed\n");
			return(-EFAULT);
		}
		printk("num = %d\n",num);
		break;
	case PWM:
		printk("pwm\n");
		ret = copy_from_user(&num, (int *)arg, sizeof(int));//arg = 3-->num = 3
		if (ret != 0)
		{
			printk("gpio_ioctl: copy_from_user failed\n");
			return(-EFAULT);
		}
		printk("num = %d\n",num);
		
		break;
	return 0;
}

static int pwm_open(struct inode *inode, struct file *filp)
{	/*初始化 GPIO 接腳*/
	
	unsigned int val;
	int ret;
	ret=gpio_request(EXYNOS4_GPX0(0),"LEDS");
	if(ret<0){
		printk(KERN_EMERG "EXYNOS4_GPX0(0) fail !\n");
		gpio_free(EXYNOS4_GPX0(0));
		return ret;
		}
	
	
	s3c_gpio_cfgpin(EXYNOS4_GPX0(0), S3C_GPIO_OUTPUT);
	//gpio_set_value(EXYNOS4_GPB(0),1);
	gpio_direction_output(EXYNOS4_GPX0(0),0);
	
	ret=gpio_request(EXYNOS4_GPX0(6),"LEDS");
	if(ret<0){
		printk(KERN_EMERG "EXYNOS4_GPX0(0) fail !\n");
		gpio_free(EXYNOS4_GPX0(6));
		return ret;
		}
	
	
	s3c_gpio_cfgpin(EXYNOS4_GPX0(6), S3C_GPIO_INPUT);
	//gpio_set_value(EXYNOS4_GPB(0),1);
	//gpio_direction_output(EXYNOS4_GPX0(6),1);

	
	return 0;
}

static int pwm_release(struct inode *inode, struct file *filp)
{
	gpio_free(EXYNOS4_GPX0(6));
	gpio_free(EXYNOS4_GPX0(0));
	return 0;
}

static struct file_operations pwm_fops = 
{
	owner		:	THIS_MODULE,
	read		:	pwm_read,
	write		:	pwm_write,
	unlocked_ioctl	:pwm_ioctl,
	open		:	pwm_open,
	release		:	pwm_release,
};

static struct class *pwm_class;

static int __init pwm_init(void)
{
	int retval;
	
	retval = register_chrdev(PWM_MAJOR,pwm_name,&pwm_fops);
	if(retval < 0)
	{
		printk(KERN_WARNING "Can't get major %d\n",PWM_MAJOR);
		return retval;
	}

	printk("PWM driver register success!\n");
	
	pwm_class = class_create(THIS_MODULE, "pwm");
        if (IS_ERR(pwm_class))
	{
		printk(KERN_WARNING "Can't make node %d\n",PWM_MAJOR);
                return PTR_ERR(pwm_class);
	}

        device_create(pwm_class, NULL, MKDEV(PWM_MAJOR, 0),NULL, pwm_name);
	printk("PWM driver make node success!\n");
	return 0;
}
static void __exit pwm_exit(void)
{
      device_destroy(pwm_class,MKDEV(PWM_MAJOR, 0));
      class_destroy(pwm_class);
	printk("PWM driver remove node success!\n");
	unregister_chrdev(PWM_MAJOR,pwm_name);
	printk("PWM driver release success!\n");

}
module_init(pwm_init);
module_exit(pwm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DMATEK Co., Ltd <http://www.dmatek.com.tw>");
MODULE_DESCRIPTION("PWM  driver");


