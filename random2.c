#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/jiffies.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");

int memory_open(struct inode *inode, struct file *filp);
int memory_release(struct inode *inode, struct file *filp);
ssize_t memory_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
void memory_exit(void);
int memory_init(void);

// Operations.
struct file_operations memory_fops = {
	read: memory_read,
};

module_init(memory_init);
module_exit(memory_exit);

// Major number.
int memory_major = 60;

int memory_init(void)
{
	int result;

	// Registering device.
	result = register_chrdev(memory_major, "random2", &memory_fops);
	if (result < 0) {
		printk("<1>random2: cannot obtain major number %d\n", memory_major);
		return result;
	}

	printk("<1>Inserting random2 module\n");
	return 0;
}

void memory_exit(void)
{
	unregister_chrdev(memory_major, "random2");

	printk("<1>Removing random2 module\n");
}

ssize_t memory_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	u64 first = get_jiffies_64();
	msleep(10);
	u64 second = get_jiffies_64();
	second = second>>32 | second<<32;
	second ^= first;
	char random[8];
	memcpy(random, &second, sizeof(second));

	copy_to_user(buf,random,sizeof(random));
	return sizeof(random);
}
