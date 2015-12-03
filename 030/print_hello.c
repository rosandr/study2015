#include <linux/module.h>
#define NIITM_NAME "hello to you, my friend from print_ko"


void printHello(void)
{
    printk(KERN_INFO "%s \n", NIITM_NAME);
}


static int niitm_init(void)
{
    return 0;
}

static void niitm_exit(void)
{
}

EXPORT_SYMBOL(printHello);

module_init(niitm_init);
module_exit(niitm_exit);
