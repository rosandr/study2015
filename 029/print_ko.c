#include <linux/module.h>
#define NIITM_NAME "niitm hello"


void print_ko(void)
{
    printk(KERN_INFO "Loaded %s \n", NIITM_NAME);
}


static int niitm_init(void)
{
    return 0;
}

static void niitm_exit(void)
{
}

EXPORT_SYMBOL(print_ko);

module_init(niitm_init);
module_exit(niitm_exit);
