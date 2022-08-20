#!/bin/bash
#$1 = vm_name

cp -R $(pwd)/hooks /etc/libvirt/hooks/qemu.d/$1/
