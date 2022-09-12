#!/bin/bash
# $1 = vm_name
      #<rom file="/etc/libvirt/hooks/qemu.d/$1/patched.rom"/>
xml_file=tmp/virsh_replacement.xml
rom_file=/etc/libvirt/hooks/qemu.d/$1/patched.rom

uuid=$(cat /etc/libvirt/qemu/$1.xml | grep uuid | sed 's/<uuid>//' | sed 's#</uuid>##' | awk '{print $1}')

xmlstarlet edit \
	--update "//domain/uuid" \
	--value $uuid $xml_file


xmlstarlet ed --insert "//hostdev[@type=pci]/address" --type elem -n rom -v "" $xml_file | xmlstarlet ed --insert //rom --type attr -n file -v $rom_file > $xml_file

      #<relaxed state="on"/>
      #<vapic state="on"/>
      #<spinlocks state="on" retries="8191"/>
      #<vpindex state="on"/>
      #<runtime state="on"/>
      #<synic state="on"/>
      #<reset state="on"/>
cat $xml_file | grep \<relaxed || xmlstarlet ed --subnode "//hyperv" --type elem -n relaxed -v "" $xml_file | xmlstarlet ed --insert //hyperv/relaxed --type attr -n state -v on > $xml_file

cat $xml_file | grep \<vapic || xmlstarlet ed --subnode "//hyperv" --type elem -n vapic -v "" $xml_file | xmlstarlet ed --insert //hyperv/vapic --type attr -n state -v on > $xml_file

cat $xml_file | grep \<spinlocks || xmlstarlet ed --subnode "//hyperv" --type elem -n spinlocks -v "" $xml_file | xmlstarlet ed --insert //hyperv/spinlocks --type attr -n state -v on | xmlstarlet ed --insert //hyperv/spinlocks --type attr -n retries -v 8191 > $xml_file

cat $xml_file | grep \<vpindex || xmlstarlet ed --subnode "//hyperv" --type elem -n vpindex -v "" $xml_file | xmlstarlet ed --insert //hyperv/vpindex --type attr -n state -v on > $xml_file

cat $xml_file | grep \<runtime || xmlstarlet ed --subnode "//hyperv" --type elem -n runtime -v "" $xml_file | xmlstarlet ed --insert //hyperv/runtime --type attr -n state -v on > $xml_file

cat $xml_file | grep \<synic || xmlstarlet ed --subnode "//hyperv" --type elem -n synic -v "" $xml_file | xmlstarlet ed --insert //hyperv/synic --type attr -n state -v on > $xml_file

cat $xml_file | grep \<reset || xmlstarlet ed --subnode "//hyperv" --type elem -n reset -v "" $xml_file | xmlstarlet ed --insert //hyperv/reset --type attr -n state -v on > $xml_file


	#<vendor_id state="on" value="default"/>
cat $xml_file | grep /<vendor_id || xmlstarlet ed --subnode "//hyperv" --type elem -n vendor_id -v "" $xml_file | xmlstarlet ed --insert //hyperv/vendor_id --type attr -n state -v on | xmlstarlet ed --insert //hyperv/vendor_id --type attr -n value -v default > $xml_file

