#include <linux/module.h>
#include "print_ko.h"
#define NIITM_NAME "niitm hello"



static int niitm_init(void)
{

    print_ko();
    //printk(KERN_INFO "Loaded %s \n", NIITM_NAME);
    return 0;
}

static void niitm_exit(void)
{
    printk(KERN_INFO "Unloaded %s \n", NIITM_NAME);
}

module_init(niitm_init);
module_exit(niitm_exit);
