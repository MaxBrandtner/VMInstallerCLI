#!/bin/bash
#$1 = input GPU list
#iommu_group=$(lspci -nn| grep '\[0300' | head -n $1 | tail -n 1)
if [ $1 -lt 1 ]
then
	exit 0
fi

iommu_group=$(lspci -nn | grep '\[0300' | head -n $1| tail -n 1 | sed 's/:.*//')

#lspci -nn | grep '\[0300' | head -n $1 | tail -n 1 | awk '{print $1}' | sed 's/:/_/' | sed 's/\./_/' | sed 's/^/pci_0000_/'

#for (( i=0; i<$(lspci | grep '$iommu_group' | wc -l); i++))
#do
lspci -nn | grep $iommu_group | awk '{print $1}' | sed 's/:/_/' | sed 's/\./_/' | sed 's/^/pci_0000_/'
#done
