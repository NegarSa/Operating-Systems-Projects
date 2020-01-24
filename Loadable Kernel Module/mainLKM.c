#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/udp.h>
#include <linux/tcp.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/inet.h>

#define DEVICE_NAME "netchar"
#define CLASS_NAME  "netLKM"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Negar Sakhaei");
MODULE_DESCRIPTION("HW2 OS - Network Kernel module");

static int majorNum;
static struct class* netLKMClass = NULL;
static struct device* netLKMBevice = NULL;
static int i = 0;
static int j = 0;
static int mode = 0;
static char ips[256][25] = {};
static struct nf_hook_ops hops;
static struct sk_buff *skb;
static struct iphdr *iph;
static struct udphdr *udph;
static struct tcphdr *tcph;
static char tbch[25] = {};

static int devv_open(struct inode*, struct file*);
static int dev_release(struct inode*, struct file*);
static ssize_t dev_read(struct file*, char*, size_t, loff_t*);
static ssize_t dev_write(struct file *, const char*, size_t, loff_t*);
unsigned int filter_func(unsigned int hooknum, struct sk_buff *skb,
                  const struct net_device *in, const struct net_device *out,
                  int(*okfn)(struct sk_buff *));

static struct file_operations fops =
{
    .open = devv_open,
    .read = dev_read,
    .write = dev_write,
    .release = dev_release,
};

unsigned int filter_func(unsigned int hooknum, struct sk_buff* s_kb,
  const struct net_device* in, const struct net_device* out,
  int (*okfn)(struct sk_buff *))
{
    skb = s_kb;
    iph = (struct iphdr *)skb_network_header(skb);
        if(!skb) return NF_ACCEPT;

        if (iph->protocol==17) {
                udph = (struct udphdr *)skb_transport_header(skb);
              //printk(KERN_INFO "Received UDP packet, ip: %pI4", &iph->saddr);
                sprintf(tbch, "%pI4:%d", &iph->saddr, ntohs(udph->source));
                int k = 0;
                for (k = 0; k < i; ++k){
                    if(strcmp(tbch, ips[k])){
                        if(!mode){
                          printk(KERN_INFO
                            "Received UDP packet, ip: %s, accepted!", tbch);
                            return NF_ACCEPT;
                        }
                        else{
                          printk(KERN_INFO
                            "Received UDP packet, ip: %s, dropped!", tbch);
                            return NF_DROP;
                        }
                    }
                }
                if(mode) {
                  printk(KERN_INFO "Received UDP packet, %s, accepted!", tbch);
                    return NF_ACCEPT;
                }
                if(!mode) {
                    printk(KERN_INFO "Received UDP packet %s, dropped!", tbch);
                    return NF_DROP;
                }
        }

        if (iph->protocol==6) {
                tcph = (struct tcphdr *)skb_transport_header(skb);
                sprintf(tbch, "%pI4:%d", &iph->saddr, ntohs(tcph->source));
                int k = 0;
                for (k = 0; k < i; ++k){
                    if(strcmp(tbch, ips[k])){
                        if(!mode){
                          printk(KERN_INFO
                            "Received TCP packet, ip: %s, accepted!", tbch);
                            return NF_ACCEPT;
                        }
                        else{
                          printk(KERN_INFO
                            "Received TCP packet, ip: %s, dropped!", tbch);
                            return NF_DROP;
                        }
                    }
                }
                if(mode) {
                  printk(KERN_INFO "Received TCP packet, %s, accepted!", tbch);
                    return NF_ACCEPT;
                }
                if(!mode) {
                    printk(KERN_INFO "Received TCP packet %s, dropped!", tbch);
                    return NF_DROP;
                }
        }
        return NF_ACCEPT;
}

static int __init LKM_init(void){
    printk(KERN_INFO "Start of init!\n");
    majorNum = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNum < 0){
        printk(KERN_INFO "registering of major number failed\n");
        return majorNum;
    }
    printk(KERN_INFO "registered with major number %d!\n", majorNum);
    netLKMClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(netLKMClass)){
       unregister_chrdev(majorNum, DEVICE_NAME);
       printk(KERN_ALERT "Failed to register device class\n");
       return PTR_ERR(netLKMClass);
    }
    printk(KERN_INFO "device class registered correctly\n");
    netLKMBevice = device_create(netLKMClass, NULL, MKDEV(majorNum, 0),
                                  NULL, DEVICE_NAME);
    if (IS_ERR(netLKMBevice)){
       class_destroy(netLKMClass);
       unregister_chrdev(majorNum, DEVICE_NAME);
       printk(KERN_ALERT "Failed to create the device\n");
       return PTR_ERR(netLKMBevice);
    }
    printk(KERN_INFO "device class created correctly\n");
    hops.hook = (nf_hookfn *) filter_func;
    hops.hooknum = NF_INET_LOCAL_IN;
    hops.pf = NFPROTO_IPV4;
    hops.priority = NF_IP_PRI_FIRST;

    nf_register_net_hook(&init_net, &hops);

    printk(KERN_INFO "initialized successfully\n");
    return 0;
}

static void __exit LKM_exit(void){
    device_destroy(netLKMClass, MKDEV(majorNum, 0));
    class_unregister(netLKMClass);
    class_destroy(netLKMClass);
    unregister_chrdev(majorNum, DEVICE_NAME);
    nf_unregister_net_hook(&init_net, &hops);
    printk(KERN_INFO "Goodbye from the LKM!\n");
}

static int devv_open(struct inode *inodep, struct file *filep){
    printk(KERN_INFO "Device has been opened!");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep){
    printk(KERN_INFO "Device successfully closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len,
                                                      loff_t *offset){
   int error_count = 0;

   error_count = copy_to_user(buffer, "data", 5);

   if (error_count==0){
      printk(KERN_INFO "Sent to the user\n");
      return 0;
   }
   else {
      printk(KERN_INFO "EBBChar: Failed to send %d characters to the user\n",
                                                      error_count);
      return -EFAULT;
   }
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len,
                                        loff_t *offset){
    if(j == 0) {
        if(strcmp(buffer, "whitelist")) mode = 0;
        else mode = 1;
        printk(KERN_INFO "Received mode %s from the user\n", buffer);
        j++;
        return len;
    }
    strncpy(ips[i], buffer, len);
    printk(KERN_INFO "Received %s from the user\n", ips[i++]);
    return len;
}

module_init(LKM_init);
module_exit(LKM_exit);
