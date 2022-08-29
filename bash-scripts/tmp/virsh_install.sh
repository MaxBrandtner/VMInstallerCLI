#!/bin/bash
virt-install --connect qemu:///system \
--name win12 \
--memory 8158 \
--disk /var/lib/libvirt/images/win12.qcow2,size=50 \
--cdrom files/input.iso \
--cpu host-model,topology.sockets=1,topology.cores=5,topology.threads=2 \
--features kvm.hidden.state=on \
--boot cdrom,fd,hd,network \
--boot loader=/usr/share/edk2-ovmf/x64/OVMF_CODE.fd,loader.readonly=yes,loader.type=pflash,nvram.template=/var/lib/libvirt/qemu/nvram/win12_VARS.fd,loader_secure=no \
--debug  --hostdev pci_0000_0a_00_0  --hostdev pci_0000_0a_00_1  --hostdev pci_0000_0a_00_2  --hostdev pci_0000_0a_00_3 \
--graphics none \
 \
--noautoconsole --install no_install=yes --wait=90
