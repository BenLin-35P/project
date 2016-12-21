#include <linux/delay.h> 
#include <linux/types.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/cdev.h>
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
#include <mach/hardware.h>
#include <linux/platform_device.h>
#include <plat/adc.h>
#include <plat/regs-adc.h>


#define adc _IOR(233,3,int)
#define adc_MAJOR 233
 int adc_minor = 0;
 int number_of_devices = 1;
#define adc_name "adc"

/*struct{
	struct s3c_adc_client *client;
	int channel;
	
}adcdev;*/
struct s3c_adc_client *client;
struct s3c_adc_client *client1;
struct cdev cdev;
        dev_t devno = 0;

int val[2];


static void adc_dev_read(void)
{	
	
	
	
	val[0]=s3c_adc_read(client,0);
	
	mdelay(100);
	val[1]=s3c_adc_read(client1,1);
	

	//printk("val=%d\nval1=%d\n",val[0],val[1]);

}



static ssize_t adc_read(struct file *filp,char *buf,size_t count,loff_t *l)
{
	
	return count;
}

static ssize_t adc_write(struct file *filp,const char *buf,size_t count,loff_t *f_ops)
{
	return count;
}

static long adc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	int num;
	

	//printk("cmd=%d\n",cmd);
	switch(cmd)
	{
	/*-------------------*/
	/* 		adc test	 	  */
	/*-------------------*/

	case 0:
		//printk("start\n");
		ret = copy_from_user(&num, (int*)arg, sizeof(int));
		if (ret != 0)
		{
			printk("adc_ioctl: copy_from_user faiorigin\n");
			return(-EFAULT);
		}
		//printk("num = %d\n",num);
		//adc_test();
		adc_dev_read();
		//printk("ioctl :val=%d\nval1=%d\n",val[0],val[1]);
		ret = copy_to_user((int*)arg,val, sizeof(val));
		if (ret != 0)
		{
			printk("adc_ioctl: copy_from_user faiorigin\n");
			return(-EFAULT);
		}
		break;	
	
	/*case adc:{
		
	
	
		
		
		break;
	}*/
	
	}
	return 0;
}

static int adc_open(struct inode *inode, struct file *filp)
{	
	

	
	return 0;
}

static int adc_release(struct inode *inode, struct file *filp)
{
	
	return 0;
}

static struct file_operations adc_fops = 
{
	owner		:	THIS_MODULE,
	read		:	adc_read,
	write		:	adc_write,
	unlocked_ioctl	:adc_ioctl,
	open		:	adc_open,
	release		:	adc_release,
};

		
	///////////////////	
static int  adc_probe( struct platform_device *pdev )
        {
                struct device *dev = &pdev->dev;
                int ret = -EINVAL;
        printk("function = %s\n", __func__);
                devno = MKDEV(adc_MAJOR, adc_minor);
        ret = register_chrdev_region(devno, number_of_devices, "adc_conversion");
                if( ret )
                {
                        dev_err(dev, "failed to register device number\n");
                        goto err_register_chrdev_region;
                }
        cdev_init(&cdev, &adc_fops);
                cdev.owner = THIS_MODULE;
                ret = cdev_add(&cdev, devno, number_of_devices);
                if( ret )
                {
                        dev_err(dev, "failed to add device\n");
                        goto err_cdev_add;
                }
        client = s3c_adc_register (pdev, NULL, NULL, 0);
        if(IS_ERR( client ))
                {
                        dev_err(dev, "failed to register adc client\n");
                        goto err_s3c_adc_register;
                }
		client1 = s3c_adc_register (pdev, NULL, NULL, 0);
        if(IS_ERR( client1 ))
                {
                        dev_err(dev, "failed to register adc client\n");
                        goto err_s3c_adc_register;
                }		

                return 0;
        err_s3c_adc_register:
                cdev_del(&cdev );
                err_cdev_add:
                unregister_chrdev_region(devno, number_of_devices);
                err_register_chrdev_region:
                return ret; 
         }
static int  adc_remove(struct platform_device *pdev)
        {
                s3c_adc_release(client);
				 s3c_adc_release(client1);
                cdev_del(&cdev );
                unregister_chrdev_region(devno, number_of_devices);
                return 0;
        }
		
		
///
	static struct platform_driver dma4412_driver_adc_con = {
                .driver = {
                        .name = "dma_adc_con",
                        .owner = THIS_MODULE,
                },
                .probe = adc_probe,
                .remove = adc_remove
        };	
		

static struct class *adc_class;

static int __init adc_init(void)
{
	
	/*int retval;
	
	retval = register_chrdev(adc_MAJOR,adc_name,&adc_fops);
	if(retval < 0)
	{
		printk(KERN_WARNING "Can't get major %d\n",adc_MAJOR);
		return retval;
	}
	
	printk("adc driver register success!\n");
	
	*/
	adc_class = class_create(THIS_MODULE, "adc");
        if (IS_ERR(adc_class))
	{
		printk(KERN_WARNING "Can't make node %d\n",adc_MAJOR);
                return PTR_ERR(adc_class);
	}

        device_create(adc_class, NULL, MKDEV(adc_MAJOR, 0),NULL, adc_name);
	printk("adc driver make node success!\n");
	
	return platform_driver_register( &dma4412_driver_adc_con );
}
static void __exit adc_exit(void)
{
	s3c_adc_release(client);
	s3c_adc_release(client1);
	
    device_destroy(adc_class,MKDEV(adc_MAJOR, 0));
    class_destroy(adc_class);
	printk("adc driver remove node success!\n");
	unregister_chrdev(adc_MAJOR,adc_name);
	printk("adc driver release success!\n");
	platform_driver_unregister(&dma4412_driver_adc_con);

}
module_init(adc_init);
module_exit(adc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("DMATEK Co., Ltd <http://www.dmatek.com.tw>");
MODULE_DESCRIPTION("adc  driver");


