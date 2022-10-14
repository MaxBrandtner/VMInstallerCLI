# Setup

## CPU virtualisation enabled

`sudo dmesg | grep -i -e DMAR -e IOMMU`

## grub arguments

`dmesg | grep "Command line:"  | head -n 1 | grep iommu`

## check GPU alone in IOMMU group

```bash
function ls_iommu_groups(){
        for d in /sys/kernel/iommu_groups/*/devices/*; do
        n=${d#*/iommu_groups/*}; n=${n%%/*}
        printf 'IOMMU Group %s ' "$n"
        lspci -nns "${d##*/}"
    done
}
```

<u>prompt user</u> if everything in IOMMU group can be passed through / is the GPU(can be checked using identifier at end line of output of `lspci -nn`)

## Check that user is in libvirt group

```bash
groups $(users)| grep libvirt
is_in_libvirt_group=$?

if [ $is_in_libvirt_group != 0 ]
then
    usermod -a -G libvirt $(users)
fi
```

## Grub

`dmesg | grep "Command line:" | head -n 1 | grep iommu`

`if [ \$? != 0] then; *do_the_following*; fi`

`amd_iommu=on iommu=pt vfio_iommu_type1.allow_unsafe_interrupts=1 amd-stibp=off clocksource=tsc tsc.reliable pci=realloc`

`or intel_iommu=on iommu=pt vfio_iommu_type1.allow_unsafe_interrupts=1 clocksource=tsc tsc.reliable pci=realloc`

`sudo grub-mkconfig -o /boot/grub/grub.cfg`

if it has to be rebuilt prompt for <u>reboot and exit</u>

## ACS Patch

<u>inform user</u> that if applied some PCI devices that communicate with each other could crash if not both are passed through

check if kernel is one of the kernels that is known to have the patch applied

`uname -r | grep zen || uname -r | grep vfio`

Linux Commandline Default: `pci_acs_override=downstream,multifunction`

## Modules

`modprobe vfio`

`echo options kvm ignore_msrs=1 >> /etc/modprobe.d/kvm.conf`

`echo options kvm report_ignored_msrs=0 >> /etc/modprobe.d/kvm.conf`

if (CPU_platform == intel)

    `echo options kvm_intel nested=1 >> /etc/modprobe.d/kvm_intel.conf`

    `modprobe -R kvm_intel`

    `modprobe kvm_intel`

if (CPU_platform == amd)

    `echo options kvm_amd nested=1 >> /etc/modprobe.d/kvm_amd.conf`

    `modprobe -R kvm_amd`

    `modprobe kvm_amd`

## libvirt

`systemctl enable --now libvirtd`

`systemctl enable --now virtlogd`

`sudo virsh net-autostart default`

`sudo virsh net-start default`

## Hugepages

`mkdir -p /dev/hugepages`

if hugepages aren't set up with fstab

`echo hugetlbfs       /dev/hugepages  hugetlbfs       mode=01770,gid=78        0 0 >> /etc/fstab`

`umount /dev/hugepages`

`mount /dev/hugepages`

## Enable KSM support

`echo 1 > /sys/kernel/mm/ksm/run`

`echo w /sys/kernel/mm/ksm/run - - - - 1 >> /etc/tmpfiles.d/ksm.conf`

## GPU preperation

### AMD

`yay -S vendor-reset-dkms-git`

`modprobe vendor-reset`

### NVIDIA

`echo blacklist i2c_nvidia_gpu >> /etc/modprobe.d/blacklist.conf`

## OVMF_VARS

### create OVMF_VARS for VM

`cp /usr/share/edk2-ovmf/x64/OVMF_VARS.fd /var/lib/libvirt/qemu/nvram/VM_NAME_VARS.fd`

### update OVMF_VARS after updating edk2-ovmf

/etc/pacman.d/hooks/ovmf_vars_update_VM_NAME.hook

```systemd
[Trigger]
Opertation=Install
Operation=Upgrade
Operation=Remove
Type=Package
Target=edk2-ovmf

[Action]
Description=Update OVMF VARS of vm VM_NAME to remain up to date
When=PostTransaction

Exec=/usr/bin/rm /var/lib/libvirt/qemu/nvram/VM_NAME_VARS.fd; /usr/bin/cp /usr/share/edk2-ovmf/x64/OVMF_VARS.fd /var/lib/libvirt/qemu/nvram/VM_NAME_VARS.fd
```

## Listing only devices that can be reset

```bash
for iommu_group in $(find /sys/kernel/iommu_groups/ -maxdepth 1 -mindepth 1 -type d);do echo "IOMMU group $(basename "$iommu_group")"; for device in $(\ls -1 "$iommu_group"/devices/); do if [[ -e "$iommu_group"/devices/"$device"/reset ]]; then echo -n "[RESET]"; fi; echo -n $'\t';lspci -nns "$device"; done; done
```

`|grep \[RESET\] to check that device can be reset`

*note if multiple PCI devices for ngle device sometimes only first one has ability to be reset(shouldn't be issue for listing possible options)*

## romfile

### option 1 extract romfile

```bash
PCI_ADDRESS_GPU=$(lspci -nn | grep 0300 | grep $selected_GPU_pci_id | sed 's/:/ /' | awk'{print $1}')
find /sys/devices -name rom | grep $PCI_ADDRESS_GPU
cd /sys/bus/pci/devices/0000:xx:xx.x

su -c 'echo 1 >rom'

cat rom  > $files_dir'/.tmp/input.rom'
```

### option 2 download rom

https://www.techpowerup.com/vgabios/

### patch romfile

**ONLY FOR NVIDIA**

https://github.com/Matoking/NVIDIA-vBIOS-VFIO-Patcher

`python nvidia_vbios_vfio_patcher.py -i \input.rom -o patched.rom`

**for AMD**

`chmod -R 660 input.rom`

## iso file

<u>prompt user</u> to download iso file or select an OS or which the ISO file is available

`mv $iso_file $files_dir"/input.iso"`

files_dir should be **/etc/libvirt/hooks/qemu.d/\$vm_name**

## MacOS adjustments

### Checks

CPU not AMD Phenom II X3 720

Nvidia GPUs only up until Maxwell(900)

AMD GPUs sometimes https://passthroughpo.st/finding-a-safe-mac-os-gpu-for-vms/

Intel iGPUs inform user that it only works sometimes 

### Setup

`pip install click request`

`git clone https://github.com/foxlet/macOS-Simple-KVM.git `

`./jumpstart.sh $mac_os_version`

**GUI** <u>check that</u> for mac VM minimum storage size is 64G

`qemu-img create -f qcow2 /var/lib/libvirt/images/$vm_name.qcow2 $storage_size"G"`

```bash
echo    -drive id=SystemDisk,if=none,file=/var/lib/libvirt/images/$vm_name.qcow2 \ >> basic.sh
echo    -device ide-hd,bus=sata.4,drive=SystemDisk \
```

### Modify Mac-VM settings

`sed -i 's/$(basic.sh | grep "-m ")/-m $memory_size"G" \\/' basic.sh`

total_threads=threads*cores

sockets=total_threads/8 + 1  *!! has to be converted to int*

`sed -i 's/$(basic.sh | grep "-smp ")/-smp cpus=$total_threads, cores=$total_threads, threads=1, sockets=$sockets \\/'' basic.sh`

modify MAC Address

`sed -i "s/$(basic.sh | grep $(awk -F "," '{print \$4}))/mac=$(openssl rand -hex 6 | sed 's/\(..\)/\1:/g; s/:$//')/" basic.sh`

### Import xml

`cd /etc/libvirt/storage; wget https://raw.githubusercontent.com/PassthroughPOST/Example-OSX-Virt-Manager/master/OSX.xml; cd -`

##### specify paths

`sed -i "s/$(cat /etc/libvirt/storage/OSX.xml | grep MyDisk)/<source file='/var/lib/libvirt/images/$vm_name.qcow2' \/>/" /etc/libvirt/storage/OSX.xml`

`sed -i 's/YOURPATH/etc\/libvirt\/hooks\/qemu.d\/$vm_name/g' /etc/libvirt/storage/OSX.xml`

##### Set topology

CPU

`sed -i 's/$(cat /etc/libvirt/storage/OSX.xml | grep topology)/<topology sockets=\'$sockets\' cores=\'$cores\' threads=\'$threads\'>/' /etc/libvirt/storage/OSX.xml`

memory_KB=\$memory_size \* 1024

`sed -i "s/$(cat /etc/libvirt/storage/OSX.xml | grep '<memory')/<memory unit=\'KIB\'>$memory_KB</memory>/" /etc/libvirt/storage/OSX.xml`

`sed -i "s/$(cat /etc/libvirt/storage/OSX.xml | grep '<currentMemory')/<currentMemory unit=\'KIB\'>$memory_KB</currentMemory>/" /etc/libvirt/storage/OSX.xml`

modify MAC Address

`sed -i "s/$(cat /etc/libvirt/storage/OSX.xml | grep '<mac')/<mac address=\'$(openssl rand -hex 6 | sed 's/\(..\)/\1:/g; s/://')\' \/>/" /etc/libvirt/storage/OSX.xml`

##### Define VM

`sudo virsh define OSX.xml`

#### Passthrough GPU

 (for AMD (and probably intel iGPUs) like windows)

disable amd passthrough for the GPUS described in this post

https://passthroughpo.st/finding-a-safe-mac-os-gpu-for-vms/

<u>inform</u> user upon selecting intel GPU that it may or may not work

 **for old Nvidia GPUs (up to 900 Series)**

<u>inside</u> of the VM

```bash
curl -O https://raw.githubusercontent.com/Benjamin-Dobell/nvidia-update/master/nvidia-update.sh
chmod 755 nvidia-update.sh
./nvidia-udpate.sh
```

https://passthroughpo.st/new-and-improved-mac-os-tutorial-part-1-the-basics/

https://github.com/foxlet/macOS-Simple-KVM.git

https://github.com/kholia/OSX-KVM

GPU passthrough + https://www.youtube.com/watch?v=miWgnT2KOjY

https://github.com/PassthroughPOST/Example-OSX-Virt-Manager

### enable apple ID

https://passthroughpo.st/prepping-osx-vm-for-apple-id/

## Vgpu unlock

NVIDIA Officially supported cards specified here: https://docs.nvidia.com/grid/latest/grid-vgpu-release-notes-vmware-vsphere/index.html#hardware-configuration

https://github.com/DualCoder/vgpu_unlock

AMD: radeonpro(no vGPU unlock because of physical differences)

### vGPU enablement on AMD

for AMD S7150 series GPUs and later

add boot parameter `module.sig_enforce=0` *(to account for host secboot)*

`git clone https://github.com/GPUOpen-LibrariesAndSDKs/MxGPU-Virtualization.git`

`cd MxGPU-Virtualization`

`make; make install`

`echo blacklist amdgpu >> /etc/modprobe.d/blacklist.conf`

`echo blacklist amdkfd >> /etc/modprobe.d/blacklist.conf`

`modprobe gim`

https://github.com/GPUOpen-LibrariesAndSDKs/MxGPU-Virtualization

after that vGPUs should appear as seperate pci devices, which can be passed through individually(*don't know if acs patching may be required*)

<u>inform user</u> that guest drivers may be unstable(kvm not officially supported) https://pve.proxmox.com/wiki/MxGPU_with_AMD_S7150_under_Proxmox_VE_5.x

### GVT-g (intel)

**since 4th gen**

add kernel parameter `i915.enable_guc=0 i915.enable_gvt=1`

`modprobe kvmgt`

`modprobe vfio-iommu-type1`

`modprobe mdev`

to check if  working `dmesg | grep -i gvt` (*if not working will say disabled*)

create `create_vGPU.sh` hook

```bash
echo "$GVT_GUID" > "/sys/devices/pci${GVT_DOM}/$GVT_PCI/mdev_supported_types/$GVT_TYPE/create"
```

create `remove-vGPU.sh` hook

```bash
echo 1 > /sys/bus/pci/devices/$GVT_PCI/$GVT_GUID/remove
```

inside `vm.xml` `<devices>`

```xml
<hostdev mode='subsystem' type='mdev' managed='no' model='vfio-pci' display='off'>
  <source>
    <address uuid=*GVT_GUID*/>
  </source>
</hostdev>
```

replace GVT_GUID with UUID of virtual GPU

`yay -S gvtg_vgpu-git` *(fixes suspend issue)*

<u>prompt user</u> to choose resolution

fix no display issue and garbled graphics issue and **set resolution**

```xml
  <qemu:commandline>
    <qemu:arg value="-set"/>
    <qemu:arg value="device.hostdev0.display=on"/>
    <qemu:env name="MESA_LOADER_DRIVER_OVERRIDE" value="i965"/>
    <qemu:arg value='-set'/>
    <qemu:arg value='device.hostdev0.xres=1440'/>
    <qemu:arg value='-set'/>
    <qemu:arg value='device.hostdev0.yres=900'/>
  </qemu:commandline>
```

https://wiki.archlinux.org/title/Intel_GVT-g

<u>prompt user</u> to install intel graphics driver on windows guest

#### vGPU unlock scripts

run this also on officially supported hardware(required for additional vGPU functionality)

nvidia-merged-dkms (aur)(on other distros you could try distrobox)

```bash
# You will need to acquire both NVIDIA-Linux-...-grid.run
# and ...-vgpu-kvm.run, place them in the same directory
# as this PKGBUILD file, and then run makepkg.
```

seem to be available on nvidia website and also here https://github.com/VGPU-Community-Drivers/NV-VGPU-Driver

to view vGPU profiles: `mdevctl types`(only vGPU types with Q at end of name relevant)

`pci_lane=0000:0a:00.0 (example)`

`for ((i=0; i<$selected_vGPUs; i++)); do mdevctl start -u $uuid[i] -p $pci_lane --type $nvidia_type; mdevctl --auto --uuid $uuid[i]; done`

if you wanted to patch the nvidia driver itself instead of having those packages use https://github.com/VGPU-Community-Drivers/vGPU-Unlock-patcher

1. apply matching IDs(2nd step README.md)

2. `./patch.sh general`

#### additional vGPU functionality

also required to be run for this to work on officially supported nvidia GPUs, therefore run vGPU unlock  scripts on officially supported hardware

https://github.com/mbilker/vgpu_unlock-rs

```bash
cd /usr/share/; git clone https://github.com/mbilker/vgpu_unlock-rs.git; cd vgpu_unlock-rs
cargo build --release

mkdir -p /etc/systemd/system/nvidia-vgpud.service.d
mkdir -p /etc/systemd/system/nvidia-vgpu-mgr.service.d
echo "[Service]" > /etc/systemd/system/nvidia-vgpud.service.d/vgpu_unlock.conf
echo "Environment=LD_PRELOAD=/usr/share/vgpu_unlock-rs/target/release/libvpu_unlock_rs.so" > /etc/systemd/system/nvidia-vgpud.service.d/vgpu_unlock.conf
echo "[Service]" > /etc/systemd/system/nvidia-vgpu-mgr.service.d/vgpu_unlock.conf
echo "Environment=LD_PRELOAD=/usr/share/vgpu_unlock-rs/target/release/libvpu_unlock_rs.so" > /etc/systemd/system/nvidia-vgpu-mgr.service.d/vgpu_unlock.conf


mkdir -p /etc/vgpu_unlock
```

/etc/vgpu_unlock/profile_override.toml

```systemd
[profile.nvidia-55]
num_displays = 1
display_width = 1920
display_height = 1080
max_pixels = 2073600
cuda_enabled = 1
frl_enabled = 0
```

for using this on multiple vGPUs add `pci_id=` (*eg 0x1B3011A0*)and `pci_device_id=`(*eg 0x1B30*) and **add as many profile.nvidia-xx as you have vGPUs** note to add framebuffers use `framebuffer=`(*eg 0xec000000*) 

when using multiple vGPUs you want to set a `frl_enabled=60`

to enable VM migration or snapshotting (nvidia driver older than *510.85.03*)

1. recompile kernel module with NV_KVM_MIGRATION_UAPI=1: nvidia-vgpu-vfio

2. `echo "unlock_migration = true" > /etc/vgpu_unlock/config.toml`

### add vGPU to VM

QUADRO_ID https://envytools.readthedocs.io/en/latest/hw/pciid.html obtain from there (also detailed here: https://github.com/VGPU-Community-Drivers/vGPU-Unlock-patcher )

`-device 'vfio-pci,sysfsdev=/sys/bus/mdev/devices/[UUID],display=off,id=hostpci0.0,bus=ich9-pcie-port-1,addr=0x0.0,x-pci-vendor-id=0x10de,x-pci-device-id=0x[QUADRO_ID],x-pci-sub-vendor-id=0x10de,x-pci-sub-device-id=0x11A0' -uuid [UUID]`

```xml
  <qemu:commandline>
    <qemu:arg value="-set"/>
    <qemu:arg value="device.hostdev0.x-pci-vendor-id=0x10de"/>
    <qemu:arg value="-set"/>
    <qemu:arg value="device.hostdev0.x-pci-device-id=quadro-equlivent-to-consumer-gpu"/>
    <qemu:arg value="-set"/>
    <qemu:arg value="device.hostdev0.x-pci-sub-vendor-id=0x10de"/>
    <qemu:arg value="-set"/>
    <qemu:arg value="device.hostdev0.x-pci-sub-device-id=quadro-equlivent-to-consumer-gpu"/>
    <qemu:arg value="-uuid"/>
    <qemu:arg value="vgpu-uuid"/>
  </qemu:commandline>
```

https://github.com/keylase/nvidia-patch/tree/master/

obtain driver version 

`nvidia-smi | grep "Driver version"" | awk '{print $3}'`

https://github.com/KrutavShah/vGPU_LicenseBypass

<u>prompt user</u> to install nvidia quadro / tesla driver

if installed **with VNC** prompt user that he / she may get multiple displays and that under load** frametimes may be not ideal**

- recommend using other local streaming services(moonlight(or parsec))

# Virt-machine stuff

## hooks

prepare/begin/start.sh

```bash
#debugging
set -x

#load variables we defined
vm_name="UNSPECIFIED_VM_NAME"
source "/etc/libvirt/hooks/qemu.d/$vm_name/kvm.conf"

#stop display manager
#systemctl start nvidia-hibernate

function lock_screen(){
    su $(users) -c 'dbus-send --type=method_call --dest=org.gnome.ScreenSaver /org/gnome/ScreenSaver org.gnome.ScreenSaver.Lock'
}

lock_screen || systemctl stop gdm || systemctl stop sddm || systemctl stop lightdm

#Unbind VTconsoles
for (( i=0; i<$(ls /sys/class/vtconsole/ | wc -l); i++ ))
do
echo 0 > /sys/class/vtconsole/vtcon$i/bind
done

#Unbind EFI-framebuffer
$N_EFI_FRAMEBUFFERS=$(ls /sys/bus/platform/drivers/efi-framebuffer | grep efi-framebuffer. | wc -l)

for (( i=0; i<$N_EFI_FRAMEBUFFERS; i++ ))
do
    echo efi-framebuffer.$i > /sys/bus/platform/drivers/efi-framebuffer/unbind
done

#avoid race condition
sleep 10

#unload Nvidia
function nvidia(){
    modprobe -r nvidia_drm
    modprobe -r nvidia_modeset
    modprobe -r drm_kms_helper
    modprobe -r nvidia
    modprobe -r i2c_nvidia_gpu
    modprobe -r nvidia_uvm
}

function amd(){
    modprobe -r amdgpu
    modprobe -r radeon
}

nvidia || amd


modprobe -r drm

#unbind gpu
N_LINES_KVM_CONF=$(cat "/etc/libvirt/hooks/qemu.d/$vm_name/kvm.conf" |grep GPU| wc -l)
if [$? == 0 ]
then
for (( i=0; i<$N_LINES_KVM_CONF; i++ ))
do
    VIRSH_VARIABLE=$(echo "VIRSH_GPU_$i")
    virsh nodedev-detach $($VIRSH_VARIABLE)
done
fi

$N_LINES_KVM_CONF=$(cat "/etc/libvirt/hooks/qemu.d/$vm_name/kvm.conf" | grep NVME | wc -l)
if [$? == 0]
then
for (( i=0; i<$N_LINES_KVM_CONF; i++ ))
do
    VIRSH_VARIABLE=$(echo "VIRSH_NVME_$i")
    virsh nodedev-detach $($VIRSH_VARIABLE)
done
fi

#virsh nodedev-detach $VIRSH_GPU_VIDEO
#virsh nodedev-detach $VIRSH_GPU_AUDIO
#virsh nodedev-detach $VIRSH_GPU_USB
#virsh nodedev-detach $VIRSH_GPU_BUS
#virsh nodedev-detach $VIRSH_USB_FRONT


#load vfio
#modprobe vfio
modprobe vfio_pci
#modprobe vfio_iommu_type1
```

release/end/revert.sh

```bash
set -x

source "/etc/libvirt/hooks/kvm.conf"

modprobe -r vfio_pci
#modprobe -r vfio_iommu_type1
#modprobe -r vfio

N_LINES_KVM_CONF=$(cat "/etc/libvirt/hooks/qemu.d/$vm_name/kvm.conf" |grep GPU| wc -l)
if [$? == 0 ]
then
for (( i=0; i<$N_LINES_KVM_CONF; i++ ))
do
    VIRSH_VARIABLE=$(echo "VIRSH_GPU_$i")
    virsh nodedev-reattach $($VIRSH_VARIABLE)
done
fi

$N_LINES_KVM_CONF=$(cat "/etc/libvirt/hooks/qemu.d/$vm_name/kvm.conf" | grep NVME | wc -l)
if [$? == 0]
then
for (( i=0; i<$N_LINES_KVM_CONF; i++ ))
do
    VIRSH_VARIABLE=$(echo "VIRSH_NVME_$i")
    virsh nodedev-reattach $($VIRSH_VARIABLE)
done
fi
#virsh nodedev-reattach $VIRSH_GPU_VIDEO
#virsh nodedev-reattach $VIRSH_GPU_AUDIO
#virsh nodedev-reattach $VIRSH_GPU_USB
#virsh nodedev-reattach $VIRSH_GPU_BUS
#virsh nodedev-reattach $VIRSH_USB_FRONT

#nvidia-xconfig --query-gpu-info > /dev/null 2>&1

for (( i=0; i<$N_EFI_FRAMEBUFFERS; i++ ))
do
    echo efi-framebuffer.$i > /sys/bus/platform/drivers/efi-framebuffer/bind
done

function nvidia(){
    modprobe nvidia_drm
    modprobe nvidia_modeset
    modprobe drm_kms_helper
    modprobe nvidia
    modprobe nvidia_uvm
    #i2c_nvidia_gpu may cause issues
}

function amd(){
    modprobe -r amdgpu
    modprobe -r radeon
}

nvidia || amd

modprobe drm

for (( i=0; i<$(ls /sys/class/vtconsole | wc -l); i++ ))
do
    echo 1 > /sys/class/vtconsole/vtcon$i/bind
done

systemctl start gdm.service || systemctl start sddm || systemctl start lightdm

dmesg >> /etc/libvirt/hooks/qemu.d/win11/dmesg-dump.log
```

### CPU pinning / CPU frequency scaling set to performance

prepare/begin/isolstart.sh

```bash
#example just allow one core
#Allowed=CPUs= core1_thread1, core1_thread_2, ...
#in reality has to be set by bash-script
systemctl set-property --runtime -- user.slice AllowedCPUs
systemctl set-property --runtime -- system.slice AllowedCPUs
systemctl set-property --runtime -- init.scope AllowedCPUs
```

release/end/isolrevert.sh

```bash
#allow for all CPU cores to be used by host
#AllowedCPUs=0-((max_thread*max_cores-1))
#has to be set by bash script
systemctl set-property --runtime -- user.slice AllowedCPUs
systemctl set-property --runtime -- system.slice AllowedCPUs
systemctl set-property --runtime -- init.scope AllowedCPUs
```

**CPU frequency schueduler not modified because modern kernel versions find it problematic + different schuedluers making it hard to write a script that takes different configurations into account**

### hugepages

https://github.com/bryansteiner/gpu-passthrough-tutorial

prepare/begin/alloc_hugepages.sh

release/end/dealloc_hugepages.sh

## kvm.conf

## create VM

chipset:q35

bios: OVMF_CODE(.secboot).fd

set cpu topology according to input

enable CDrom and sata disk

## xml modifications

romfile in GPU config vm.xml

### Optimize VFIO passthrough VMs

`sed -i "s/$(cat /etc/libvirt/storage/$vm_name'.xml' | grep '<memballoon') /<memballoon model="none\/>" /etc/libvirt/storage/$vm_name'.xml'`

### hyperv enlightenments

```xml
      <relaxed state="on"/>
      <vapic state="on"/>
      <spinlocks state="on" retries="8191"/>
      <vendor_id state="on" value="kvm hyperv"/>
      <vpindex state='on'/>
      <synic state='on'/>
      <stimer state='on'/>
      <reset state='on'/>
      <frequencies state='on'/>
      <vendor_id state='on' value='12charval'>
```

## USB redirection

<u>give option</u> to pass through USB controller

<u>give option</u> to pass through USB device

https://wiki.archlinux.org/title/PCI_passthrough_via_OVMF#Passing_keyboard/mouse_via_Evdev

<u>give option</u> to automatically pass USB devices through with udev

https://wiki.archlinux.org/title/QEMU(Automatic USB forwarding with udev)

## Emulating GPUs

Standard / default(2d graphics)

`--graphics qxl`

3d on mondern linux guests(for windows user special version / patch windows iso)

URL for Virtio windows iso:`https://fedorapeople.org/groups/virt/virtio-win/direct-downloads/stable-virtio/virtio-win.iso`

Alternatively install on guest guest driver provided here: `https://github.com/virtio-win/virtio-win-pkg-scripts/blob/master/README.md`

`--graphics virtio`

VNC(on port 5900 + input)

`--graphics vnc`

## enabling audio on windows for emulated graphics and vGPU

install VBcable driver(for vGPU as well)

## Install Looking Glass

`yay -S looking-glass`

also handles audio not working properly(no VBCable required)

inside `vm.xml` `<devices>`

```xml
<shmem name='looking-glass'>
  <model type='ivshmem-plain'/>
  <size unit='M'>32</size>
</shmem>
```

Memory size(in MB) = `(width x height x 4 x 2) / 1024 / 1024 + 10`

`sed -i 's/<input type=\'tablet\'>//' $vm_name'.xml'`

add `<input type='mouse'/>``<input type='keyboard' bus='virtio'/>`

if apparmor is installed: `echo /dev/shm/looking-glass rw, >> /etc/apparmor.d/local/abstractions/libvirt-qemu; systemctl restart apparmor`

install looking glass on guest

### looking glass clipboard sync

```xml
<channel type="spicevmc">
  <target type="virtio" name="com.redhat.spice.0"/>
  <address type="virtio-serial" controller="0" bus="0" port="1"/>
</channel>
<!-- No need to add a VirtIO Serial device, it will be added automatically -->
```

for guest download looking-glass from https://looking-glass.hostfission.com/downloads
