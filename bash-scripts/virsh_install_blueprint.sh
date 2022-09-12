#!/bin/bash
virt-install --connect qemu:///system \
--name  vm_name --arch=x86_64 --virt-type=kvm --print-xml=1 --machine=q35 \
--memory memory_size \
--disk /var/lib/libvirt/images/vm_name.qcow2,size=storage_size \
--cdrom iso_path \
--cpu host-model,topology.sockets=n_cpu_sockets,topology.cores=n_cpu_cores,topology.threads=n_cpu_threads \
--features kvm.hidden.state=on \
--boot cdrom,fd,hd,network \
--boot loader=/usr/share/edk2-ovmf/x64/OVMF_CODE.enabled_secboot.fd,loader.readonly=yes,loader.type=pflash,nvram.template=/var/lib/libvirt/qemu/nvram/ vm_name_VARS.fd,loader_secure=enabled_secboot_yes_no \
paravirt_options
--accelerate \
--nonetworks \
--noautoconsole \
enabled_tpm \
