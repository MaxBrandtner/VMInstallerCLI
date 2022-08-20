#!/bin/bash
# $1 = gpu_number
touch /etc/libvirt/hooks/kvm.conf

while IFS= read -r line
do
	echo "VIRSH_GPU_$iterator=$line" >> /etc/libvirt/hooks/kvm.conf

	((iterator++))
done < $(bash pci_ids.sh $1)

