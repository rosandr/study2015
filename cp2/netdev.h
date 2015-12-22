#ifndef _INCLUDE_CHARDEV_H_
#define _INCLUDE_CHARDEV_H_

char DEVNAME[]="kbuf";
#define DEV_MAJOR 60

//получение статистики
#define IOCTL_GET_STAT _IOR(DEV_MAJOR, 0, ssize_t *)
//сброс статистики
#define IOCTL_RESET_STAT _IOR(DEV_MAJOR, 1, ssize_t *)
//получение списка процессов
#define IOCTL_GET_PROCLIST _IOR(DEV_MAJOR, 2, ssize_t *)
//получение ce статистики
#define IOCTL_GET_NETSTAT _IOR(DEV_MAJOR, 3, ssize_t *)

typedef struct
{
    int open_cnt;
    int close_cnt;
    int read_cnt;
    int write_cnt;
    int seek_cnt;
    int ioctl_cnt;
    int irq_cnt;
}DEV_STAT;


typedef struct
{
    struct net_device* dev;
    int txmit_count;
} MYNET_DEV;



#endif
