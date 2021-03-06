#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include <linux/kernel.h>
#include <linux/bitops.h>
#include <linux/device.h>
#include <linux/etherdevice.h>

#include "netdev.h"

#define KBUF_LOADED "kbuf loaded"
#define BUF_SIZE PAGE_SIZE
#define IRQ_NUM 17

//typedef irqreturn_t (*irq_handler_t)(int, void*);

int dev_major = DEV_MAJOR;
int dev_minor = 1;
int dev_count=1;

char* buf=NULL;     // work buffer

dev_t dev_node;
struct cdev* my_dev=NULL;
static DEV_STAT dev_stat;

struct net_device* my_netdev=NULL;
NETDEV_PRIV* my_netdev_priv=NULL;

//========================================================
int mynet_dev_open(struct net_device *dev)		// ifconfig up
{
    return 0;
}


int mynet_dev_stop(struct net_device *dev)		// ifconfig down
{
    return 0;
}


netdev_tx_t mynet_dev_start_xmit (struct sk_buff *skb, struct net_device *dev)
{
//    struct mynet_dev* priv = my_netdev_priv(dev);
    my_netdev_priv->txmit_count++;
    memset(my_netdev_priv->buf, 0, 255);

    if(skb->len > 256)
        memcpy(my_netdev_priv->buf, skb->data, 256);
    else
        memcpy(my_netdev_priv->buf, skb->data, skb->len);

    printk(KERN_INFO "netdev xmit count=%d\n", my_netdev_priv->txmit_count);
    dev_kfree_skb_any(skb);

    return NETDEV_TX_OK;
}


void mynet_dev_tx_timeout(struct net_device *dev)
{
    return;
}


static const struct net_device_ops mynet_dev_ops=
{
    .ndo_open = mynet_dev_open,
    .ndo_stop = mynet_dev_stop,
    .ndo_start_xmit = mynet_dev_start_xmit,
    .ndo_tx_timeout = mynet_dev_tx_timeout,
};


static void mynet_dev_init( struct net_device* dev)
{
    my_netdev_priv = netdev_priv(dev);
    my_netdev_priv->dev = dev;
    my_netdev_priv->txmit_count=0;

    memset(my_netdev_priv->buf, 0, sizeof( my_netdev_priv->buf));

    ether_setup(dev);
    dev->watchdog_timeo = 100;
    dev->netdev_ops = &mynet_dev_ops;
    dev->flags |= IFF_NOARP;
}

/*
//========================================================
irqreturn_t chardev_irq (int val, void* pp)
{
    dev_stat.irq_cnt++;
    return IRQ_NONE;
}
*/
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
    //printk(KERN_INFO "chardev lseek()\n");
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
    printk(KERN_INFO "chardev lseek(%d)\n", (int)*ppos);
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
    int offset=0;
    int ret =-1;
    struct task_struct* init_task = current;
    struct task_struct *task = init_task;

    printk(KERN_INFO "chardev ioctl() \n");
    dev_stat.ioctl_cnt++;

    switch (cmd)
    {
    case IOCTL_GET_NETSTAT:
        if( copy_to_user((void __user *)param, (void*)my_netdev_priv, sizeof( NETDEV_PRIV)))
            printk(KERN_ERR "chardev IOCTL_GET_NETSTAT failed\n");
        else ret=0;
        break;

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
/*
        do
        {
          printk( KERN_INFO "chardev IOCTL_GET_PROCLIST: %s [%d] parent %s\n",
                         task->comm, task->pid, task->parent->comm );
        } while ( (task = next_task(task)) != init_task );
*/
        do
        {
            snprintf((char*)buf+offset, BUF_SIZE-offset, "task name = %s, \t\tpid=%d\n", task->comm, task->pid);
            offset += sizeof("task name = , \t\tpid=\n")+sizeof(task->comm) + sizeof(task->pid);
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
    // 3. ------- implement internal buffer
    buf = kzalloc( BUF_SIZE, GFP_KERNEL);

    if(buf == NULL)
    {
        printk(KERN_ERR "kmalloc failed\n" );
        return -ENOMEM;
    }

    printk(KERN_INFO "chardev loaded\n");

    // 2. ------- add device...         >>>ls -la /dev/kbuf
    dev_node = MKDEV( dev_major, dev_minor);        // bind to node in /dev/kbuf (node /dev/kbuf must exist)

/**/
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
        return -ENODEV;
    }
/*
    //-------------------------------------------------------
    if (request_irq( IRQ_NUM, chardev_irq, IRQF_SHARED, "chardev_handler", &dev_major))
    {
        unregister_chrdev_region( dev_node, dev_count );
        printk(KERN_ERR "IRQ registration chardev failed\n");
	return -1;
    }
*/

    //-------------------------------------------------------
    my_netdev = alloc_netdev (sizeof(NETDEV_PRIV), "mynet0",
                              /*NET_NAME_UNKNOWN,*/ mynet_dev_init);

    if( my_netdev == NULL)
    {
        unregister_chrdev_region( dev_node, dev_count );
        printk(KERN_ERR "Netdev create failed\n");
        return -ENODEV;
    }

    if(register_netdev(my_netdev))
    {
        free_netdev(my_netdev);
        unregister_chrdev_region( dev_node, dev_count );
        printk(KERN_ERR "Netdev create failed\n");
        return -ENODEV;
    }

    memset((void*)&dev_stat, 0, sizeof( dev_stat));
    printk(KERN_INFO "Registration chardev (netdev) succeed\n");

    return 0;
}

static void chardev_exit(void)
{
//    synchronize_irq( IRQ_NUM );
//    free_irq( IRQ_NUM, NULL);		//

    if(my_netdev)
    {
        unregister_netdev(my_netdev);
        free_netdev(my_netdev);
    }

    if(my_dev)  cdev_del(my_dev);

    unregister_chrdev_region( dev_node, dev_count );

    printk(KERN_INFO "chardev unloaded\n");
    kfree(buf);
}

module_init(chardev_init);
module_exit(chardev_exit);


MODULE_LICENSE("GPL");
MODULE_VERSION( "0.1" );
