#!/bin/bash
#$1 = vm_name

cp -R hooks tmp/

sed -i 's/_INSERT_VM_NAME_/$1/' tmp/hooks/prepare/begin/start.sh

sed -i 's/_INSERT_VM_NAME_/$1/' tmp/hooks/release/end/revert.sh


mkdir -p /etc/libvirt/hooks/qemu.d/$1

ls /etc/libvirt/hooks/qemu.d/$1/hooks
hooks_exists=$?
if [ hooks_exists != 0 ]
then
	cp -R tmp/hooks /etc/libvirt/hooks/qemu.d/$1/hooks
fi