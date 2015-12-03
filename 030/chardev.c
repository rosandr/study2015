#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>

#define KBUF_LOADED "kbuf loaded"



char DEVNAME[]="kbuf";


ssize_t chardev_read (struct file * fd, char __user* addr, size_t size, loff_t* loff)
{
    return 0;
}

ssize_t chardev_write (struct file* fd, const char __user* addr, size_t size, loff_t* loff)
{

    return 0;
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
    printk(KERN_INFO "chardev loaded\n");
    //static struct file_operations* fops;


    // 1. ------- registration (determ.  fops apriori cat /proc/devices  )
    //int res = 
    register_chrdev (60, DEVNAME, &fops);

    // 2. ------- add device  ls -la /dev/kbuf
    dev_t dev_node;
    dev_node = MKDEV( 60, 1);
    register_chrdev_region(dev_node, 1, DEVNAME);

    struct cdev* my_dev;
    my_dev = cdev_alloc();
    cdev_init(my_dev, &fops);
    cdev_add(my_dev, dev_node, 1);

    return 0;
}

static void chardev_exit(void)
{
//    int res = 
    unregister_chrdev (60, DEVNAME);
    printk(KERN_INFO "chardev unloaded\n");
}

module_init(chardev_init);
module_exit(chardev_exit);
