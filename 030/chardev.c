#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/slab.h>


#define KBUF_LOADED "kbuf loaded"
#define BUF_SIZE PAGE_SIZE


char DEVNAME[]="kbuf";
int dev_major = 60;
int dev_minor = 1;
int dev_count=1;
char* buf=NULL;


dev_t dev_node;
struct cdev* my_dev=NULL;


ssize_t chardev_read (struct file* fd, char __user* user, size_t size, loff_t* loff)
{
    printk(KERN_INFO "read from chardev\n");
    return 0;
}

ssize_t chardev_write (struct file* fd, const char __user* addr, size_t size, loff_t* loff)
{
    printk(KERN_INFO "write to chardev\n");
    return 0;
}

int chardev_open (struct inode* in, struct file* fd)
{
    printk(KERN_INFO "chardev open \n");
    return 0;
}

int chardev_release (struct inode* in, struct file* fd)
{
    printk(KERN_INFO "chardev close \n");
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
    int res;

    // 3. ------- implement internal buffer
    buf = kmalloc( BUF_SIZE, GFP_KERNEL);

    if(buf == NULL)
    {
        printk(KERN_ERR "kmalloc failed\n" );
        return -ENOMEM;
    }

    printk(KERN_INFO "chardev loaded\n");

    // 1. ------- registration... (determ.  fops apriori)           >>>cat /proc/devices
    res = register_chrdev (dev_major, DEVNAME, &fops);

    if( res < 0 )
    {
        printk(KERN_ERR "Registration chardev failed\n");
        return res;
    }
    printk(KERN_INFO "Registration chardev succeed\n");


    // 2. ------- add device...         >>>ls -la /dev/kbuf
    dev_node = MKDEV( dev_major, dev_minor);        // bind to node in /dev/kbuf (node /dev/kbuf must exist)
    register_chrdev_region( dev_node, dev_count, DEVNAME);
/**/
    my_dev = cdev_alloc();

    if(my_dev == NULL)
    {
        printk(KERN_ERR "cdev_alloc failed\n" );
        return -ENOMEM;
    }

    cdev_init(my_dev, &fops);

    if (cdev_add(my_dev, dev_node, 1) == -1)
    {
        unregister_chrdev_region( dev_node, dev_count );
        return -1;
    }

    return 0;
}

static void chardev_exit(void)
{
    if(my_dev)  cdev_del(my_dev);

    unregister_chrdev_region( dev_node, dev_count );

    //    int res =
    unregister_chrdev (dev_major, DEVNAME);


    printk(KERN_INFO "chardev unloaded\n");
    kfree(buf);
}

module_init(chardev_init);
module_exit(chardev_exit);
