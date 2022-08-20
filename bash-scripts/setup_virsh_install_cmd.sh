#!/bin/bash
# $1 	= vm_name
# $2 	= memory_size(MB)
# $3 	= storage_size(GB)
# $4 	= iso_path(full path)
# $5 	= n_cpu_sockets
# $6 	= n_cpu_cores
# $7 	= n_cpu_threads
# $8 	= enabled_secboot
# $9 	= paravirtualized
# $10	= enabled_tmp
# $11	= gpu_number



sed 's/vm_name/'$1'/' <virsh_install.sh >virsh_install.sh
sed 's/memory_size/'$2'/' <virsh_install.sh >virsh_install.sh
sed 's/iso_path/'$3'/' <virsh_install.sh >virsh_install.sh
sed 's/n_cpu_sockets/'$4'/' <virsh_install.sh >virsh_install.sh
sed 's/n_cpu_cores/'$5'/' <virsh_install.sh >virsh_install.sh
sed 's/n_cpu_threads/'$6'/' <virsh_install.sh >virsh_install.sh



if [ $8 == 1 ]
then
        sed 's/enabled_secboot/secboot/' <virsh_install.sh >virsh_install.sh
        sed 's/enabled_secboot_yes_no/yes' <virsh_install.sh >virsh_install.sh
else
	sed 's/.enabled_secboot//' <virsh_install.sh >virsh_install.sh
	sed 's/enabled_secboot_yes_no/no' <virsh_install.sh >virsh_install.sh
fi



if [ $9 == 1 ]
then
	
	while IFS= read -r line
	do
		sed 's/paravirt_options/ --hostdev '$line' paravirt_options' <virsh_install.sh >virsh_install.sh

		((iterator++))
	done < $(bash pci_ids.sh $11)
fi

sed 's/paravirt_options/\\/'



if [ $10 == 1 ]
then
	sed 's/enabled_tmp/--tpm emulator/' <virsh_install.sh >virsh_install.sh
else
	sed 's/enabled_tmp//'
fi
