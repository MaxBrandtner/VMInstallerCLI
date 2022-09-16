#!/bin/bash
# $1 = vm_name
# $2 = gpu_number
ls /etc/libvirt/hooks | grep kvm.conf
return_value=$?

cat .tmp/pci_ids || bash pci_ids.sh ${10} >> .tmp/pci_ids

if [ $return_value -eq 0 ]
then
	touch /etc/libvirt/hooks/$1/kvm.conf

	while IFS= read -r line
	do
		echo "VIRSH_GPU_$iterator=$line" >> /etc/libvirt/hooks/$1/kvm.conf

		((iterator++))
done <.tmp/pci_ids

