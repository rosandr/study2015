#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define KBUF_LOADED "kbuf loaded"
#define BUF_SIZE PAGE_SIZE


char DEVNAME[]="kbuf";

char* buf;
loff_t rd_offset=0, wr_offset=0;
size_t buflen=BUF_SIZE;


ssize_t chardev_read (struct file* fd, char __user* user, size_t size, loff_t* loff)
{
    //size_t len, rest;

    printk(KERN_INFO "reading from chardev\n");
/*
    rest = BUF_SIZE - rd_offset;
    len = min(size, rest);

    if(len == 0)
	return 0;

    if( copy_to_user(user, buf+rd_offset, len) !=0 )
	return -EFAULT;

    //rd_offset += len;
    *loff = rd_offset;

    return len;
*/
    return -EINVAL;
}

ssize_t chardev_write (struct file* fd, const char __user* user, size_t size, loff_t* loff)
{
//    size_t len, rest;

    printk(KERN_INFO "writing to chardev\n");
/*
    rest = BUF_SIZE - wr_offset;
    len = min(size, rest);

    if(len == 0)
	return 0;

    if( copy_from_user(buf+wr_offset, user, len) !=0 )
	return -EFAULT;

    wr_offset += len;
    *loff = wr_offset;

    return len;

*/
    return -EINVAL;
}

int chardev_open (struct inode * in, struct file * fd)
{



    return 0;
}

int chardev_release (struct inode* in, struct file* fd)
{


    return 0;
}

static const struct file_operations fops=
{
    .owner = THIS_MODULE,
    .read = chardev_read,
    .write = chardev_write,
    .open = chardev_open,
    .release = chardev_release,
};

static int chardev_init(void)
{
    dev_t dev_node;
    struct cdev* my_dev;

    printk(KERN_INFO "chardev loaded\n");
    //static struct file_operations* fops;


    // 1. ------- registration (determ.  fops apriori cat /proc/devices  )
    //int res = 
    register_chrdev (60, DEVNAME, &fops);

    // 2. ------- add device  ls -la /dev/kbuf
    dev_node = MKDEV( 60, 1);
    register_chrdev_region(dev_node, 1, DEVNAME);

    my_dev = cdev_alloc();
    cdev_init(my_dev, &fops);
    cdev_add(my_dev, dev_node, 1);

    // 3. ------- implement internal buffer
    //buf = kmalloc( BUF_SIZE, GFP_KERNEL);

    //if(buf == NULL)	return -1;
    return 0;
}

static void chardev_exit(void)
{
    //kfree(buf);
//    int res = 
    unregister_chrdev (60, DEVNAME);
    printk(KERN_INFO "chardev unloaded\n");
}

module_init(chardev_init);
module_exit(chardev_exit);
