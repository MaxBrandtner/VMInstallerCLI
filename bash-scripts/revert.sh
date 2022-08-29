#!/bin/bash
#$1 = vm_name
rm /var/lib/libvirt/images/$1.qcow2
rm -r /etc/libvirt/hooks/qemu.d/$1
rm /etc/libvirt/qemu/$1.xml
