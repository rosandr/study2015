#include <linux/module.h>
#include "print_hello.h"
#define NIITM_NAME "niitm hello"



static int niitm_init(void)
{
    printk(KERN_INFO "niitm_hello loaded\n");
    printHello();

    return 0;
}

static void niitm_exit(void)
{
    printk(KERN_INFO "Unloaded %s \n", NIITM_NAME);
}

module_init(niitm_init);
module_exit(niitm_exit);
