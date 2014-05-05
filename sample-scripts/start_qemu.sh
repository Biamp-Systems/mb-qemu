#!/bin/bash

sudo ../mb-qemu/microblaze-softmmu/qemu-system-microblaze -machine labx-microblaze-devicetree,dtb=labrinth-avb.dtb -kernel ../mb-linux/uClinux-dist/linux-2.6.x/vmlinux -m 256 -serial /dev/tty -s -net nic,macaddr=00:0A:35:00:22:01 -net nic,macaddr=00:0A:35:00:22:02 -net user -net socket,mcast=230.0.0.1:2468 -net dump,file=foo.pcap -redir tcp:2471::23 -pflash flash@87000000 -nographic -monitor telnet:127.0.0.1:1233,server,nowait -net nic -net tap,ifname=tap0
