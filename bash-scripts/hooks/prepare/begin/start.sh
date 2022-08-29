#!/bin/bash
# adapted from https://stackoverflow.com/questions/22009364/is-there-a-try-catch-command-in-bash

function try()
{
    [[ $- = *e* ]]; SAVED_OPT_E=$?
    set +e
}

function catch()
{
    export ex_code=$?
    (( $SAVED_OPT_E )) && set +e
    return $ex_code
}



#debugging
set -x

#load variables we defined
source "/etc/libvirt/hooks/_INSERT_VM_NAME_/kvm.conf"

#stop possible display managers
try(
	systemctl stop gdm
)
catch || {
	try(
		systemctl stop sddm
	)
	catch || {
		try(
			systemctl stop lightdm
		)
	}
}

#Unbind VTconsoles
for vt_console_count in $(/sys/class/vtconsole/ ls -1 | wc -l)
do
echo 0 > /sys/class/vtconsole/vtcon$vt_console_count/bind
done

#Unbind EFI-framebuffer
echo efi-framebuffer.0 > /sys/bus/platform/drivers/efi-framebuffer/unbind

#avoid race condition
sleep 10

#unload Nvidia
try(
	lspci | grep NVIDIA || exit 1
	
	modprobe -r nvidia_drm
	modprobe -r nvidia_modeset
	modprobe -r drm_kms_helper
	modprobe -r nvidia
	modprobe -r i2c_nvidia_gpu
	modprobe -r nvidia_uvm
)

try(
	lspci | grep Radeon || exit 1

	modprobe -r amdgpu
	modprobe -r radeon
)

modprobe -r drm


#unbind gpu
while IFS= read -r line
do
	virsh nodedev-detach $[VIRSH_GPU_$line]

	((iterator++))
done < /etc/libvirt/hooks/kvm.conf



#load vfio
modprobe vfio
modprobe vfio_pci
modprobe vfio_iommu_type1
