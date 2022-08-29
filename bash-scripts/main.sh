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

if [ $(whoami) != 'root' ]
then
	echo "not root user"
	exit 2
fi

usermod -a -G libvirt $(who | awk '{print $1}')

mkdir -p tmp

echo "############"
echo "#copy files#"
echo "############"
bash copy_files.sh $2 $9
echo "################"
echo "#grub arguments#"
echo "################"
bash grub_arguments.sh

echo "#################"
echo "#install romfile#"
echo "#################"
bash install_romfile.sh $8


echo "#########################"
echo "#setup virsh install cmd#"
echo "#########################"
bash setup_virsh_install_cmd.sh $1 $5 $6 0 $3 $4 ${10} $7 ${11} $8

echo "###############"
echo "#virsh install#"
echo "###############"
bash tmp/virsh_install.sh
virsh destroy $1


echo "###########"
echo "#xml edits#"
echo "###########"
bash xml_edits.sh $1

echo "##########################"
echo "#isolcpu script generator#"
echo "##########################"
bash isolcpu_script_generator.sh $3
echo "#################"
echo "#hooks generator#"
echo "#################"
bash hooks_generator.sh $1
#bash kvm_conf_generator.sh $1 $8


echo "##################"
echo "#remove tmp files#"
echo "##################"
#rm -R files
#rm -R tmp
