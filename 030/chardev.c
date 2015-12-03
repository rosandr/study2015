#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/slab.h>


#define KBUF_LOADED "kbuf loaded"
#define BUF_SIZE 256


char DEVNAME[]="kbuf";

char* buf;


ssize_t chardev_read (struct file* fd, char __user* addr, size_t size, loff_t* loff)
{
    printk(KERN_INFO "read from chardev\n");
    return EINVAL;
}

ssize_t chardev_write (struct file* fd, const char __user* addr, size_t size, loff_t* loff)
{
    printk(KERN_INFO "write to chardev\n");
    return EINVAL;
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
    buf = kmalloc( PAGE_SIZE, GFP_KERNEL);

    if(buf == NULL)

    return 0;
}

static void chardev_exit(void)
{
    kfree(buf);
//    int res = 
    unregister_chrdev (60, DEVNAME);
    printk(KERN_INFO "chardev unloaded\n");
}

module_init(chardev_init);
module_exit(chardev_exit);
