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


set -x

source "/etc/libvirt/hooks/_INSERT_VM_NAME_/kvm.conf"

modprobe -r vfio_pci
modprobe -r vfio_iommu_type1
modprobe -r vfio

while IFS= read -r line
do
        virsh nodedev-reattach $[VIRSH_GPU_$line]

        ((iterator++))
done < /etc/libvirt/hooks/kvm.conf



for vt_console_count in $(/sys/class/vtconsole/ ls -1 | wc -l)
do
	if [ vt_console_count ==0 ]
	then
		echo 1 > /sys/class/vtconsole/vtcon$vt_console_count/bind
	else
		echo 0 > /sys/class/vtconsole/vtcon$vt_console_count/bind
	fi
done



try(
	lspci | grep NVIDIA || exit 1
	nvidia-xconfig --query-gpu-info > /dev/null 2>&1
)



echo "efi-framebuffer.0" > /sys/bus/platform/drivers/efi-framebuffer/bind



try(    
        lspci | grep NVIDIA || exit 1
        
        modprobe nvidia_drm
        modprobe nvidia_modeset
        modprobe drm_kms_helper
        modprobe nvidia
        modprobe i2c_nvidia_gpu
        modprobe nvidia_uvm
)       

try(
        lspci | grep Radeon || exit 1

        modprobe amdgpu
        modprobe radeon
)

modprobe drm



#start possible display managers
try(
        systemctl start gdm
)
catch || {
        try(
                systemctl start sddm
        )
        catch || {
                try(
                        systemctl start lightdm
                )
        }
}


sleep 90

xrandr | grep Screen
xrandr_return=$?

if [ $xrandr_return -eq 0]
then
	echo "it worked"
else
	reboot
fi

