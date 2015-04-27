/*
 * pci_driver.c

 *
 *  Created on: Apr 25, 2015
 *      Author: james
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/init.h>


#define my_vendor_id 0x1337
#define my_device_id 0x1337
#define my_pci_driver_name "james_pci_driver"

// set how many BAR registers the device implements
#define bars_implemented 1;

// BAR addresses from kernel; up to 6 can be used
unsigned long bar0_start;
unsigned long bar0_end;
#if bars_implemented > 1
  unsigned long bar1_start;
  unsigned long bar1_end;
#if bars_implemented > 2
  unsigned long bar2_start;
  unsigned long bar2_end;
#if bars_implemented > 3
  unsigned long bar3_start;
  unsigned long bar3_end;
#if bars_implemented > 4
  unsigned long bar4_start;
  unsigned long bar4_end;
#if bars_implemented > 5
  unsigned long bar5_start;
  unsigned long bar5_end;
#endif // >5
#endif // >4
#endif // >3
#endif // >2
#endif // >1


static struct pci_device_id ids[] = {
    { my_vendor_id, PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
    { 0, }
};

MODULE_DEVICE_TABLE(pci, ids);


static int probe(struct pci_dev *dev, const struct pci_device_id *id)
{
  printk(KERN_DEBUG "%s:probe() called\n", my_pci_driver_name);

  u8 vendorId, deviceId;
  int i;

  // actually enable the PCI card
  pci_enable_device(dev);

  // read configuration space and check if we got correct device
  pci_read_config_byte(dev, my_vendor_id, &vendorId);
  pci_read_config_byte(dev, my_device_id, &deviceId);
  if(vendorId != my_vendor_id) {
    printk(KERN_ERROR"%s:probe() Wrong device! Expecting vendor %X and device %X;", my_pci_driver_name, my_vendor_id, my_device_id);
    printk(KERN_ERROR"Received vendor %X and device %X;\n", vendorId, deviceId);
    return -1;
  }

  // set up access the device's memory
  printk(KERN_DEBUG "%s:probe(): Requesting %d BAR locations\n", bars_implemented);
  bar0_start = pci_resource_start(dev, 0);
  unsigned long bar0_end;
  #if bars_implemented > 1
    unsigned long bar1_start;
    unsigned long bar1_end;
  #if bars_implemented > 2
    unsigned long bar2_start;
    unsigned long bar2_end;
  #if bars_implemented > 3
    unsigned long bar3_start;
    unsigned long bar3_end;
  #if bars_implemented > 4
    unsigned long bar4_start;
    unsigned long bar4_end;
  #if bars_implemented > 5
    unsigned long bar5_start;
    unsigned long bar5_end;
  #endif // >1
  #endif // >2
  #endif // >3
  #endif // >4
  #endif // >5



  return 0;
}


static void remove(struct pci_dev *dev)
{
/* clean up any allocated resources and stuff here.
* like call release_region();
*/
}

static struct pci_driver my_pci_driver = {
  .name = my_pci_driver_name,
  .id_table = ids,
  .probe = probe,
  .remove = remove,
};

static int __init my_pci_driver_init(void)
{
  return pci_register_driver(&my_pci_driver);
}

static void __exit my_pci_driver_exit(void)
{
  pci_unregister_driver(&my_pci_driver);
}

MODULE_LICENSE("GPL");
module_init(my_pci_driver_init);
module_exit(my_pci_driver_exit);
