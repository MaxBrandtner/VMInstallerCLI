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
# $11	= generate hostdev xml

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

virsh_install_dir='.tmp/virsh_install.sh'

if [ ${11} == 1 ]
then
	virsh_install_dir='.tmp/virsh_install_print_xml.sh'
fi

mkdir -p tmp
cp virsh_install_blueprint.sh $virsh_install_dir

sed 's/vm_name/'$1'/' <virsh_install_blueprint.sh > $virsh_install_dir

if [ ${11} == 1 ]
then
	echo " "
else
	sed -i 's/--print-xml=1//' $virsh_install_dir
fi

sed -i 's/vm_name/'$1'/' $virsh_install_dir
sed -i 's/ '$1'/'$1'/' $virsh_install_dir
sed -i 's/memory_size/'$2'/' $virsh_install_dir
sed -i 's/storage_size/'$3'/' $virsh_install_dir
sed -i 's#iso_path#files/input.iso#' $virsh_install_dir
sed -i 's/n_cpu_sockets/'$n_cpu_sockets'/' $virsh_install_dir
sed -i 's/n_cpu_cores/'$5'/' $virsh_install_dir
sed -i 's/n_cpu_threads/'$6'/' $virsh_install_dir

if [ $7 == 1 ]
then
        sed -i 's/enabled_secboot/secboot/' $virsh_install_dir
        sed -i 's/enabled_secboot_yes_no/yes --features smm.state=on/' $virsh_install_dir
else
	sed -i 's/.enabled_secboot//' $virsh_install_dir
	sed -i 's/enabled_secboot_yes_no/no/' $virsh_install_dir
fi

pwd

if [ $8 != 0 ]
then
	if [ ${11} == 1 ]
	then
		#sed -i 's/paravirt_options/--paravirt paravirt_options/' tmp/virsh_install.sh
		#sed -i 's/paravirt_options/--virt-type=kvm paravirt_options/' tmp/virsh_install.sh
		cat tmp/pci_ids || bash pci_ids.sh ${10} >> tmp/pci_ids
	
		while IFS= read -r line
		do
			sed -i 's/paravirt_options/ --hostdev='$line' paravirt_options/' $virsh_install_dir
			sed -i 's/--accelerate/--check path_in_use=off/' $virsh_install_dir

			((iterator++))
		done <tmp/pci_ids
	fi
fi

sed -i 's/paravirt_options/\\/' $virsh_install_dir



if [ $9 == 1 ]
then
	sed -i 's/enabled_tpm/--tpm emulator/' $virsh_install_dir
else
	sed -i 's/enabled_tpm//' $virsh_install_dir
fi

cp /usr/share/edk2-ovmf/x64/OVMF_VARS.fd /var/lib/libvirt/qemu/nvram/$1_VARS.fd
modprobe vfio
modprobe vfio-pci

cat $virsh_install_dir
