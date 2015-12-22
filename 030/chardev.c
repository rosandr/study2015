#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include <linux/sched.h>
#include <linux/interrupt.h>

#include "chardev.h"

#define KBUF_LOADED "kbuf loaded"
#define BUF_SIZE PAGE_SIZE
//  #define IRQ_NUM 47      ---> see chardev.h

int dev_major = DEV_MAJOR;
int dev_minor = 1;
int dev_count=1;

char* buf=NULL;     // work buffer
dev_t dev_node;

struct cdev* my_dev=NULL;
static DEV_STAT dev_stat;

//========================================================
irqreturn_t chardev_irq (int val, void* pp)
{
    dev_stat.irq_cnt++;
    return IRQ_NONE;
}

//========================================================
/*
 * fd - откуда
 * буфер для возврата данных
 * размер буфера
 * off ??
*/
ssize_t chardev_read (struct file* fd, char __user* user, size_t len, loff_t* off)
{
    int nbytes;
    int rest;
    printk(KERN_INFO "read from chardev\n" );
    dev_stat.read_cnt++;

    rest = BUF_SIZE - *off;
    nbytes = (rest < len) ? rest:len;

    nbytes -= copy_to_user( user, buf + *off, nbytes);
    *off+=nbytes;
    
    return nbytes;
}

/*
 * Файловая операция
 * fd - куда
 * буфер для полученных данных
 * размер данных
 * off ??
*/
ssize_t chardev_write (struct file* file, const char __user* user, size_t len, loff_t* off)
{
    int nbytes;
    int rest;

    printk( KERN_INFO "write to chardev\n");
    dev_stat.write_cnt++;

    if( *off > BUF_SIZE )  return -EINVAL;      // too big offset

    rest = BUF_SIZE - *off;
    nbytes = (rest < len) ? rest:len;

    nbytes -= copy_from_user((void *)(buf + *off ), user, nbytes);
    *off += nbytes;
    
    return nbytes;
}


loff_t chardev_seek (struct file* file, loff_t off, int whence)
{
    loff_t* ppos;
    printk(KERN_INFO "chardev lseek()\n");
    dev_stat.seek_cnt++;

    ppos = &file->f_pos;

    switch( whence )
    {
    case SEEK_SET:      // от начала
        if( off > BUF_SIZE || off < 0 )
        {
            printk(KERN_ERR "chardev SEEK_SET : Invalid offset!\n");
            return -EOVERFLOW;
        }
        *ppos = off;
        break;

    case SEEK_CUR:      // от текущей
        if( (off > (BUF_SIZE- *ppos)) || (off < (0-*ppos)) )
        {
            printk(KERN_ERR "chardev SEEK_CUR : Invalid offset!\n");
            return -EOVERFLOW;
        }
        *ppos += off;
        break;

    case SEEK_END:      // от конца
        if( off > 0 || off < (0-BUF_SIZE) )
        {
            printk(KERN_ERR "chardev SEEK_END : Invalid offset!\n");
            return -EOVERFLOW;
        }

        *ppos = off + BUF_SIZE;
        break;
    default:
        return -EINVAL;
    }
    return *ppos;
}

int chardev_open (struct inode* in, struct file* fd)
{
    printk(KERN_INFO "chardev open() \n");
    dev_stat.open_cnt++;
    return 0;
}

int chardev_release (struct inode* in, struct file* fd)
{
    printk(KERN_INFO "chardev close() \n");
    dev_stat.close_cnt++;
    return 0;
}

long chardev_ioctl( struct file* fd, unsigned int cmd, unsigned long param)
{
    //int offset=0;
    int ret =-1;
    struct task_struct* init_task = current;
    struct task_struct *task = init_task;

    printk(KERN_INFO "chardev ioctl() \n");
    dev_stat.ioctl_cnt++;

    switch (cmd)
    {
    case IOCTL_GET_STAT:
        if( copy_to_user((void __user *)param, (void*)&dev_stat, sizeof( dev_stat)))
            printk(KERN_ERR "chardev IOCTL_GET_STAT failed\n");
        else ret=0;
        break;
    case IOCTL_RESET_STAT:
        memset((void*)&dev_stat, 0, sizeof( dev_stat));
        ret =0;
        break;
    case IOCTL_GET_PROCLIST:
        snprintf((char*)buf, BUF_SIZE, "task name = %s, \t\tpid=%d\n", task->comm, task->pid);
        /*
        do
        {
            snprintf((char*)buf+offset, BUF_SIZE-offset, "task name = %s, \t\tpid=%d\n", task->comm, task->pid);
            offset += sizeof("task name = , \t\tpid=\n")+sizeof(task->comm) + sizeof(task->pid);
        } while ( (task = next_task(task)) != init_task );*/
        break;
    default:
        break;
    }
    return ret;
}


static const struct file_operations fops=
{
    .owner = THIS_MODULE,
    .llseek = chardev_seek,
    .read = chardev_read,
    .write = chardev_write,
    .open = chardev_open,
    .release = chardev_release,
    .unlocked_ioctl = chardev_ioctl,
};

static int chardev_init(void)
{
     // 3. ------- implement internal buffer
    buf = kzalloc( BUF_SIZE, GFP_KERNEL);

    if(buf == NULL)
    {
        printk(KERN_ERR "kmalloc failed\n" );
        return -ENOMEM;
    }

    printk(KERN_INFO "chardev loaded\n");
/*
    // 1. ------- registration... (determ.  fops apriori)           >>>cat /proc/devices
    res = register_chrdev (dev_major, DEVNAME, &fops);
    if( res < 0 )
    {
        printk(KERN_ERR "Registration chardev failed\n");
        return res;
    }
*/
    // 2. ------- add device...         >>>ls -la /dev/kbuf
    dev_node = MKDEV( dev_major, dev_minor);        // bind to node in /dev/kbuf (node /dev/kbuf must exist)

    register_chrdev_region( dev_node, dev_count, DEVNAME);

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
        printk(KERN_ERR "Registration chardev failed\n");
        return -1;
    }

    //-------------------------------------------------------
    if (request_irq( IRQ_NUM, chardev_irq, IRQF_SHARED, "chardev_handler", &dev_major))
    {
        unregister_chrdev_region( dev_node, dev_count );
        printk(KERN_ERR "IRQ registration chardev failed\n");
        return -1;
    }

    memset((void*)&dev_stat, 0, sizeof( dev_stat));
    printk(KERN_INFO "Registration chardev succeed\n");

    return 0;
}

static void chardev_exit(void)
{
    synchronize_irq( IRQ_NUM );
    free_irq( IRQ_NUM, &dev_major);		//

    if(my_dev)  cdev_del(my_dev);

    unregister_chrdev_region( dev_node, dev_count );
    //unregister_chrdev (dev_major, DEVNAME);

    printk(KERN_INFO "chardev unloaded\n");
    kfree(buf);
}

module_init(chardev_init);
module_exit(chardev_exit);


MODULE_LICENSE("GPL");
MODULE_VERSION( "0.1" );
