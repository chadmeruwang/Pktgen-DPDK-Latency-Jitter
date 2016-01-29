#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x9a31bb74, "module_layout" },
	{ 0x35b6b772, "param_ops_charp" },
	{ 0xf55bb238, "pci_unregister_driver" },
	{ 0xaba33759, "__pci_register_driver" },
	{ 0xe2d5255a, "strcmp" },
	{ 0xb529515a, "_dev_info" },
	{ 0x7be51c40, "pci_enable_msix" },
	{ 0xad436042, "dev_notice" },
	{ 0x468f09e9, "__uio_register_device" },
	{ 0x12a0df79, "sysfs_create_group" },
	{ 0xafbf3787, "pci_intx_mask_supported" },
	{ 0xcfc62f27, "xen_start_info" },
	{ 0x731dba7a, "xen_domain_type" },
	{ 0x2eb84610, "dma_supported" },
	{ 0xe7ae0ca3, "dma_set_mask" },
	{ 0x42c8de35, "ioremap_nocache" },
	{ 0xd9acbbd9, "pci_set_master" },
	{ 0xfee0571a, "dev_err" },
	{ 0x2252cdbc, "pci_request_regions" },
	{ 0xace5f3c3, "pci_enable_device" },
	{ 0xd61adcbd, "kmem_cache_alloc_trace" },
	{ 0x5cd9dbb5, "kmalloc_caches" },
	{ 0x7633ea35, "pci_check_and_mask_intx" },
	{ 0xf73a9160, "pci_intx" },
	{ 0x3bb967f5, "pci_cfg_access_unlock" },
	{ 0xf90fd742, "pci_cfg_access_lock" },
	{ 0x1ac46550, "remap_pfn_range" },
	{ 0x964bcb2, "boot_cpu_data" },
	{ 0x27e1a049, "printk" },
	{ 0xc0b33b22, "pci_disable_msix" },
	{ 0x37a0cba, "kfree" },
	{ 0xb3be75f6, "dev_set_drvdata" },
	{ 0x86db9a2d, "pci_disable_device" },
	{ 0xe6959bc, "pci_release_regions" },
	{ 0xedc03953, "iounmap" },
	{ 0x8a0921ab, "uio_unregister_device" },
	{ 0xcb7122d2, "sysfs_remove_group" },
	{ 0x10519fe3, "dev_get_drvdata" },
	{ 0x28318305, "snprintf" },
	{ 0xc06ff079, "pci_enable_sriov" },
	{ 0x83d41938, "pci_num_vf" },
	{ 0x4eb83c82, "pci_disable_sriov" },
	{ 0x3c80c06c, "kstrtoull" },
	{ 0xbdfb6dbb, "__fentry__" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=uio";


MODULE_INFO(srcversion, "CA181C4E966FF7974598041");
