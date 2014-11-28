Enable busmastering.

Create file runqemu-ifup and scripts (thing my dad sent) -- no longer need sudo for qemu
1000 1000 3 /

tcpdump -xx -e -i tap0

install tuctl (uml-utilities)

change sbin

ping  -c1 92.168.7.2

 sudo qemu-system-x86_64 -net nic,model=rtl8139 -net tap,ifname=tap0 --serial mon:stdio -hda ~/Downloads/linux-0.2.img

qemu-system-x86_64 -net nic,model=rtl8139 -net tap,ifname=tap0 --serial mon:stdio -hdc kernel/kernel.img -hdd fat439/user.img


arp -a (to see mac address cache)
sudo arp -s <address> <max>
