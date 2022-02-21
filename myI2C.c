#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include <linux/io.h>

MODULE_AUTHOR("yehuh");
MODULE_DESCRIPTION("led driver");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL;
static volatile u32* i2c_base = NULL;

typedef union {
	u32 All_Bits
	struct{
		unsigned bit0:1;
		unsigned bit1:1;
		unsigned bit2:1;
		unsigned bit3:1;
		unsigned bit4:1;
		unsigned bit5:1;
		unsigned bit6:1;
		unsigned bit7:1;
		unsigned bit8:1;
		unsigned bit9:1;
		unsigned bit10:1;
		unsigned bit11:1;
		unsigned bit12:1;
		unsigned bit13:1;
		unsigned bit14:1;
		unsigned bit15:1;
		unsigned bit16:1;
		unsigned bit17:1;
		unsigned bit18:1;
		unsigned bit19:1;
		unsigned bit20:1;
		unsigned bit21:1;
		unsigned bit22:1;
		unsigned bit23:1;
		unsigned bit24:1;
		unsigned bit25:1;
		unsigned bit26:1;
		unsigned bit27:1;
		unsigned bit28:1;
		unsigned bit29:1;
		unsigned bit30:1;
		unsigned bit31:1;
	}
}uniByte;
static volatile uniByte* C_Reg		= NULL;
static volatile uniByte* S_Reg		= NULL;
static volatile uniByte* DLEN_Reg	= NULL;
static volatile uniByte* A_Reg		= NULL;
static volatile uniByte* FIFO_Reg	= NULL;
static volatile uniByte* DIV_Reg	= NULL;
static volatile uniByte* DEL_Reg	= NULL;
static volatile uniByte* CLKT_Reg	= NULL;

const u8 LcdAddr = 0x40;
static ssize_t i2c_write(struct file* filp, const char* buf, size_t count, loff_t* pos){
	char c_buff;
	if(copy_from_user(&c_buff, buf, sizeof(char))){
		return -EFAULT;
	}
	if(c_buff == '0'){
		gpio_base[10] = 1 << 25;//set 25 th bit in 0x28 (clear output)
	}else if(c_buff == '1'){
		gpio_base[7] = 1 << 25; //set 25 th bit in 0x1C (set out put)
	}

	return 1;
}

static struct file_operations led_fops = {
	.owner = THIS_MODULE,
	.write = led_write
};

static int __init init_mod(void){
	int retval;
	u32 i2c_base_addr = 0x3F205000;
	u8 region_of_map = 0x20;
	i2c_base = ioremap(i2c_base_addr, region_of_map);

	C_Reg	= i2c_base[0];
	c_Reg.bit15 =1; //enable i2c
	S_Reg	= i2c_base[1];
	DLEN_Reg= i2c_base[2];
	A_Reg	= i2c_base[3];
	FIFO_Reg= i2c_base[4];
	DIV_Reg = i2c_base[5];
	DEL_Reg = i2c_base[6];
	CLKT_Reg= i2c_base[7];


	retval = alloc_chrdev_region(&dev, 0, 1, "myI2C");
	if(retval < 0){
		printk(KERN_ERR "alloc_chrdev fail!!.\n");
		return retval;
	}
	cdev_init(&cdv, &led_fops);
	cdv.owner = THIS_MODULE;
	retval = cdev_add(&cdv, dev, 1);

	if(retval < 0){
		printk(KERN_ERR "acdev_add() fail. major:%d, minor:%d\n", MAJOR(dev), MINOR(dev));
		return retval;
	}
	printk(KERN_INFO "%s is loaded. major:%d.\n",__FILE__,MAJOR(dev));

	cls = class_create(THIS_MODULE,"myled");
	if(IS_ERR(cls)){
		printk(KERN_ERR "class_create() fail.");
		return PTR_ERR(cls);
	}
	device_create(cls, NULL, dev, NULL, "myled%d", MINOR(dev));
	return 0;
}

static void __exit cleanup_mod(void){
	cdev_del(&cdv);
	device_destroy(cls, dev);
	class_destroy(cls);
	unregister_chrdev_region(dev, 1);
	printk(KERN_INFO "%s is unloaded. major:%d.\n",__FILE__,MAJOR(dev));
}

module_init(init_mod);
module_exit(cleanup_mod);
