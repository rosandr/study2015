#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include <linux/sched.h>

#include "chardev.h"

#define KBUF_LOADED "kbuf loaded"
#define BUF_SIZE PAGE_SIZE

//char DEVNAME[]="kbuf";
int dev_major = DEV_MAJOR;
int dev_minor = 1;
int dev_count=1;

char* buf=NULL;     // work buffer
int cur_pos=0;

dev_t dev_node;
struct cdev* my_dev=NULL;

//========================================================
/*
 * fd - откуда
 * буфер для возврата данных
 * размер буфера
 * off ??
*/
ssize_t chardev_read (struct file* fd, char __user* user, size_t user_len, loff_t* off)
{
    int rd_len= cur_pos;
    printk(KERN_INFO "read from chardev\n");

    if( user_len < cur_pos )  return -EINVAL;      // too litle user buffer
/*
    if( *off != 0 )     return 0;           // EOF

    if( copy_to_user( user, buf, rd_len ) ) return -EFAULT;

    *off = rd_len;
    cur_pos=0;
    dev_stat.read_cnt++;

    return rd_len;
    */

    if( cur_pos == 0 )     return 0;           // EOF

    if( copy_to_user( user, buf, rd_len ) ) return -EFAULT;

    *off=cur_pos=0;
    dev_stat.read_cnt++;

    return rd_len;
}


/*
 * fd - куда
 * буфер для полученных данных
 * размер данных
 * off ??
*/
ssize_t chardev_write (struct file* fd, const char __user* user, size_t size, loff_t* off)
{
    int rest = BUF_SIZE-cur_pos;

    printk(KERN_INFO "write to chardev\n");

    if (size > rest )   return -EINVAL;       // too long data

    if(copy_from_user((void *)(buf + cur_pos), user, size))
    {
        printk(KERN_ERR "copy_from_user() failed\n");
        return -EFAULT;
    }
    cur_pos += size;
    dev_stat.write_cnt++;

    *off=cur_pos;
    return size;
}


loff_t chardev_seek (struct file* fd, loff_t off, int whence)
{
    if( off > (BUF_SIZE-1) || off < 0 )
    {
        printk(KERN_ERR "lseek() SEEK_SET : Invalid offset!\n");
        return -EOVERFLOW;
    }

    printk(KERN_INFO "chardev lseek()\n");

    switch( whence )
    {
    case SEEK_SET:      // от начала
        cur_pos=off;
        break;

    case SEEK_CUR:      // от текущей

        if( off > (BUF_SIZE-1-cur_pos) )
        {
            printk(KERN_ERR "lseek() SEEK_CUR : Invalid offset!\n");
            return -EOVERFLOW;
        }
        cur_pos+=off;

        break;
    case SEEK_END:      // от конца
        if( off > 0 || off < (1-BUF_SIZE) )
        {
            printk(KERN_ERR "lseek() SEEK_END : Invalid offset!\n");
            return -EOVERFLOW;
        }

        cur_pos = off + (BUF_SIZE-1);
        break;
    default:
        return -EINVAL;
    }

    dev_stat.seek_cnt++;
    return cur_pos;
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
    int ret =-1;
    struct task_struct* init_task = current;
    struct task_struct *task = init_task;

    printk(KERN_INFO "chardev ioctl() \n");

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
        //printk( KERN_INFO "Current task is %s [%d]\n", task->comm, task->pid );

        do
        {
          printk( KERN_INFO "chardev IOCTL_GET_PROCLIST: %s [%d] parent %s\n",
                         task->comm, task->pid, task->parent->comm );
        } while ( (task = next_task(task)) != init_task );
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
    int res;

    // 3. ------- implement internal buffer
    buf = kzalloc( BUF_SIZE, GFP_KERNEL);

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

    memset((void*)&dev_stat, 0, sizeof( dev_stat));

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
