#!/bin/bash
virsh install \
--connect qemu:///system \
--name vm_name \
--memory memory_size \
--disk /etc/libvirt/qemu/images/vm_name.qcow2,size=storage_size \
--cdrom iso_path \
--cpu host-model,topology.sockets=n_cpu_sockets,topology.cores=n_cpu_cores,topology.threads=n_cpu_threads \
--features kvm.hidden.state=on \
--boot cdrom,fd,hd,network loader=/usr/share/edk2-ovmf/x64/OVMF_CODE.enabled_secboot.fd,loader.readonly=yes,loader.type=pflash,nvram.template=/var/lib/libvirt/qemu/nvram/vm_name_VARS.fd,loader_secure=enabled_secboot_yes_no \
paravirt_options
enabled_tmp
