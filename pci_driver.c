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
#include <linux/types.h>

#define my_vendor_id 0x1337
#define my_device_id 0x1337
#define my_pci_driver_name "james_pci_driver"

// set how many BAR registers the device implements
#define bars_implemented 1

// BAR addresses from kernel; up to 6 can be used
unsigned long bar_start[bars_implemented];
unsigned long bar_end[bars_implemented];

// pointers to BAR virtual memory map buffers

static struct pci_device_id ids[] = {
    { my_vendor_id, PCI_ANY_ID, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0 },
    { 0, }
};

MODULE_DEVICE_TABLE(pci, ids);

size_t dmaBufferSize;
dma_addr_t dmaBufferPointer;

/*
 * Use the probe() function to:
 *  claim the PCI memory
 *  setup PCI configuration registers
 *  enable the device
 */
static int probe(struct pci_dev *dev, const struct pci_device_id *id)
{
  uint8_t vendorId;
  uint8_t deviceId;
  int i;

  printk(KERN_DEBUG "%s:probe() called\n", my_pci_driver_name);


  // actually enable the PCI card
  if(!pci_enable_device(dev)) {
    printk(KERN_ERR "%s:probe():pci_enable_device failed!\n", my_pci_driver_name);
    return -1;
  }

  // read configuration space and check if we got correct device
  pci_read_config_byte(dev, my_vendor_id, &vendorId);
  pci_read_config_byte(dev, my_device_id, &deviceId);
  if(vendorId != my_vendor_id) {
    printk(KERN_ERR"%s:probe() Wrong device! Expecting vendor %X and device %X;", my_pci_driver_name, my_vendor_id, my_device_id);
    printk(KERN_ERR"Received vendor %X and device %X;\n", vendorId, deviceId);
    return -1;
  } else {
    printk(KERN_DEBUG"Read Vendor ID:%X and Device ID:%X;\n", vendorId, deviceId);
  }

  // let device be a bus master for dma and such
  pci_set_master(dev);

  // set up access the device's memory
  printk(KERN_DEBUG "%s:probe(): Requesting %d BAR locations\n", my_pci_driver_name, bars_implemented);
  for(i=0;i<bars_implemented;i++) {
    bar_start[i] = pci_resource_start(dev, i);
    bar_end[i] = pci_resource_end(dev, i);
  }

  // this will reserve all memory on the PCI card
  if(!pci_request_regions(dev, my_pci_driver_name)) {
    printk(KERN_ERR"%s:probe(): pci_request_regions failed!\n", my_pci_driver_name);
  }

  // for dma - TODO!
//  pci_dma_set_mask()
//  pci_alloc_consistent(dev, dmaBufferSize, dmaBufferPointer);


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


module_init(my_pci_driver_init);
module_exit(my_pci_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Big Sak");

