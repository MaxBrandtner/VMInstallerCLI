#!/bin/bash
# $1 	= vm_name
# $2 	= memory_size(MB)
# $3 	= storage_size(GB)
# $4 	= iso_path(full path)
# $5 	= n_cpu_cores
# $6 	= n_cpu_threads
# $7 	= enabled_secboot
# $8 	= paravirtualized
# $9	= enabled_tmp
# $10	= gpu_number

echo $1
echo $2
echo $3
echo $4
echo $5
echo $6
echo $7
echo $8
echo $9
echo ${10}

#only needed for macos OS - selection not yet implemented
#n_cpu_sockets=$((1+$5*$6))
n_cpu_sockets=1

mkdir -p tmp
cp virsh_install_blueprint.sh tmp/virsh_install.sh

sed 's/vm_name/'$1'/' <virsh_install_blueprint.sh >tmp/virsh_install.sh
sed -i 's/vm_name/'$1'/' tmp/virsh_install.sh
sed -i 's/ '$1'/'$1'/' tmp/virsh_install.sh
sed -i 's/memory_size/'$2'/' tmp/virsh_install.sh
sed -i 's/storage_size/'$3'/' tmp/virsh_install.sh
sed -i 's#iso_path#files/input.iso#' tmp/virsh_install.sh
sed -i 's/n_cpu_sockets/'$n_cpu_sockets'/' tmp/virsh_install.sh
sed -i 's/n_cpu_cores/'$5'/' tmp/virsh_install.sh
sed -i 's/n_cpu_threads/'$6'/' tmp/virsh_install.sh

if [ $7 == 1 ]
then
        sed -i 's/enabled_secboot/secboot/' tmp/virsh_install.sh
        sed -i 's/enabled_secboot_yes_no/yes --features smm.state=on/' tmp/virsh_install.sh
else
	sed -i 's/.enabled_secboot//' tmp/virsh_install.sh
	sed -i 's/enabled_secboot_yes_no/no/' tmp/virsh_install.sh
fi

pwd

if [ $8 == 1 ]
then
	cat tmp/pci_ids || bash pci_ids.sh ${10} >> tmp/pci_ids
	
	while IFS= read -r line
	do
		sed -i 's/paravirt_options/ --hostdev '$line' paravirt_options/' tmp/virsh_install.sh

		((iterator++))
	done <tmp/pci_ids
fi

sed -i 's/paravirt_options/\\/' tmp/virsh_install.sh



if [ $9 == 1 ]
then
	sed -i 's/enabled_tpm/--tpm emulator/' tmp/virsh_install.sh
else
	sed -i 's/enabled_tpm//' tmp/virsh_install.sh
fi

cp /usr/share/edk2-ovmf/x64/OVMF_VARS.fd /var/lib/libvirt/qemu/nvram/$1_VARS.fd
modprobe vfio
modprobe vfio-pci

cat tmp/virsh_install.sh
