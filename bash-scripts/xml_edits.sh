#!/bin/bash
# $1 = vm_name
      #<rom file="/etc/libvirt/hooks/qemu.d/$1/patched.rom"/>
xmlstarlet ed --insert "//hostdev[@type=pci]/address" --type elem -n rom -v "" /etc/libvirt/qemu/$1.xml | xmlstarlet ed --insert //rom --type attr -n file -v /etc/libvirt/hooks/qemu.d/$1/patched.rom > /etc/libvirt/qemu/$.xml

      #<relaxed state="on"/>
      #<vapic state="on"/>
      #<spinlocks state="on" retries="8191"/>
      #<vpindex state="on"/>
      #<runtime state="on"/>
      #<synic state="on"/>
      #<reset state="on"/>
cat /etc/libvirt/qemu/$1.xml | grep \<relaxed || xmlstarlet ed --subnode "//hyperv" --type elem -n relaxed -v "" /etc/libvirt/qemu/$1.xml | xmlstarlet ed --insert //hyperv/relaxed --type attr -n state -v on > /etc/libvirt/qemu/$1.xml

cat /etc/libvirt/qemu/$1.xml | grep \<vapic || xmlstarlet ed --subnode "//hyperv" --type elem -n vapic -v "" /etc/libvirt/qemu/$1.xml | xmlstarlet ed --insert //hyperv/vapic --type attr -n state -v on > /etc/libvirt/qemu/$1.xml

cat /etc/libvirt/qemu/$1.xml | grep \<spinlocks || xmlstarlet ed --subnode "//hyperv" --type elem -n spinlocks -v "" /etc/libvirt/qemu/$1.xml | xmlstarlet ed --insert //hyperv/spinlocks --type attr -n state -v on | xmlstarlet ed --insert //hyperv/spinlocks --type attr -n retries -v 8191 > /etc/libvirt/qemu/$1.xml

cat /etc/libvirt/qemu/$1.xml | grep \<vpindex || xmlstarlet ed --subnode "//hyperv" --type elem -n vpindex -v "" /etc/libvirt/qemu/$1.xml | xmlstarlet ed --insert //hyperv/vpindex --type attr -n state -v on > /etc/libvirt/qemu/$1.xml

cat /etc/libvirt/qemu/$1.xml | grep \<runtime || xmlstarlet ed --subnode "//hyperv" --type elem -n runtime -v "" /etc/libvirt/qemu/$1.xml | xmlstarlet ed --insert //hyperv/runtime --type attr -n state -v on > /etc/libvirt/qemu/$1.xml

cat /etc/libvirt/qemu/$1.xml | grep \<synic || xmlstarlet ed --subnode "//hyperv" --type elem -n synic -v "" /etc/libvirt/qemu/$1.xml | xmlstarlet ed --insert //hyperv/synic --type attr -n state -v on > /etc/libvirt/qemu/$1.xml

cat /etc/libvirt/qemu/$1.xml | grep \<reset || xmlstarlet ed --subnode "//hyperv" --type elem -n reset -v "" /etc/libvirt/qemu/$1.xml | xmlstarlet ed --insert //hyperv/reset --type attr -n state -v on > /etc/libvirt/qemu/$1.xml


	#<vendor_id state="on" value="default"/>
cat /etc/libvirt/qemu/$1.xml | grep /<vendor_id || xmlstarlet ed --subnode "//hyperv" --type elem -n vendor_id -v "" /etc/libvirt/qemu/$1.xml | xmlstarlet ed --insert //hyperv/vendor_id --type attr -n state -v on | xmlstarlet ed --insert //hyperv/vendor_id --type attr -n value -v default > /etc/libvirt/qemu/$1.xml

