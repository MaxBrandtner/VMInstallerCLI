#!/bin/bash
# adapted from https://stackoverflow.com/questions/22009364/is-there-a-try-catch-command-in-bash

function try()
{
    [[ $- = *e* ]]; SAVED_OPT_E=$?
    set +e
}

function throw()
{
    exit $1
}

function catch()
{
    export ex_code=$?
    (( $SAVED_OPT_E )) && set +e
    return $ex_code
}

function throwErrors()
{
    set -e
}

function ignoreErrors()
{
    set +e
}





export IommuNotFound=100

try
(
    	echo "	#################################"
	echo "	#check for grub iommu parameters#"
	echo "	#################################"
	echo " "
    	cat /etc/default/grub | grep CMDLINE_LINUX_DEFAULT| grep iommu || throw $IommuNotFound

    	# make sure to clear $ex_code, otherwise catch * will run
	echo ""
    	echo "	####################################"
	echo "	#Found iommu grub cmdline parameter#"
	echo "	####################################"
)

catch || {
    	echo "	############################"
	echo "	#check for cpu manufacturer#"
	echo "	############################"
	echo " "
    	case $ex_code in
        	$IommuNotFound)
 
		export AMDCpuNotFound=101
		#get line CMD_LINE_LINUX_DEFAULT is on
                #cudos https://stackoverflow.com/questions/20026370/using-bash-script-to-find-line-number-of-string-in-file/47541176
		grub_file_cmd_args_line=grep -wn "CMD_LINE_LINUX_DEFAULT" /etc/default/grub | cut -d: -f1
        	try
		(
			lscpu | grep AMD || throw $AMDCpuNotFound
			#echo "AMD"
			mv /etc/default/grub /etc/default/grub_vm_install.bak
cat /etc/default/grub_vm_install.bak | sed 's/GRUB_CMDLINE_LINUX_DEFAULT=\"/GRUB_CMDLINE_LINUX_DEFAULT=\"amd_iommu=on iommu=pt vfio_iommu_type1.allow_unsafe_interrupts=1 /' >> /etc/default/grub

		)
		catch || {
			#echo "intel"
			mv /etc/default/grub /etc/default/grub_vm_install.bak
cat /etc/default/grub_vm_install.bak | sed 's/GRUB_CMDLINE_LINUX_DEFAULT=\"/GRUB_CMDLINE_LINUX_DEFAULT=\"intel_iommu=on iommu=pt vfio_iommu_type1.allow_unsafe_interrupts=1 /' >> /etc/default/grub

		}

		grub-mkconfig -o /boot/grub/grub.cfg
		notify-send "VMInstallerCLI" "After install a reboot will be necessary before using the vm"

        	;;
    	esac
}
