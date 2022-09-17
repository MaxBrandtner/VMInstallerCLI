#!/bin/bash
# $1 = selected gpu
n_iommu_devices=$(lspci | grep $(lspci -nn | grep '\[0300\]' | sed 's/:/ /' | awk '{print $1}' | head -n $1 | tail -n 1) | wc -l)
n_iommu_gpu_devices=$(lspci -nn | grep $(lspci -nn | grep '\[0300\]' | sed 's/:/ /' | awk '{print $1}' | head -n $1 | tail -n 1) | grep 300 | wc -l)


if [ $n_immu_devices == $n_iommu_gpu_devices ]:
then
	exit 0
else
	exit 1
fi
