#! /bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $SCRIPT_DIR

# $1 = vm_name
# $2 = iso_path
# $3 = selected_cpu_sockets
# $4 = selected_cpu_cores
# $5 = selected_cpu_threads
# $6 = memory_size(MB)
# $7 = storage_size(GB)
# $8 = paravirtualized
# $9 = gpu_number
# $10 = input_romfile
# $11 = enabled_secboot
# $12 = enabled_tpm

if [ whoami != 'root' ]
then
	exit 2
fi

bash grub_arguments.sh
bash install_romfile.sh	$10 $9



bash setup_virsh_install_cmd.sh $1 $6 $7 $2 $3 $4 $5 $11 $12 $13 $14

bash virsh_install.sh

bash xml_edits.sh $9


bash isolcpu_script_generator.sh $4
bash hooks_generator.sh $1
bash kvm_conf_generator.sh $9
