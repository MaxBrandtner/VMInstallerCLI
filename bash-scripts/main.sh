#! /bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd $SCRIPT_DIR

# $1 = vm_name
# $2 = iso_path
# $3 = selected_cpu_cores
# $4 = selected_cpu_threads
# $5 = memory_size(MB)
# $6 = storage_size(GB)
# $7 = paravirtualized
# $8 = gpu_number
# $9 = input_romfile
# $10 = enabled_secboot
# $11 = enabled_tpm

if [ whoami != 'root' ]
then
	exit 2
fi

usermod -a -G libvirt $(who | awk '{print $1}')

bash copy_files.sh $2 $9
bash grub_arguments.sh
bash install_romfile.sh $9



bash setup_virsh_install_cmd.sh $1 $5 $6 $(echo "\"$(pwd)/files/input.iso\"") $3 $4 $10 $7 $11 $8

bash virsh_install.sh

bash xml_edits.sh $1


bash isolcpu_script_generator.sh $3
bash hooks_generator.sh $1
bash kvm_conf_generator.sh $8
