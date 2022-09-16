#! /bin/bash
# $1 = selected_gpu_number

#ls ~/.local/share/VMInstaller-CLI || mkdir ~/.local/share/VMInstaller-CLI
#ls ~/.local/share/VMInstaller-CLI/install_process || mkdir ~/.local/share/VMInstaller-CLI/install_process
#cd ~/.local/share/VMInstaller-CLI/install_process

mkdir -p .tmp
mkdir -p .tmp/hooks
mkdir -p .tmp/files
cd .tmp/files
mkdir -p git-clones
cd git-clones

lspci -nn | grep '\[0300' | head -n $1 | tail -n 1 | grep NVIDIA
return_value=$?
if [ $return_value -eq 0 ]
then
  tree 'NVIDIA-vBIOS-VFIO-Patcher' ||git clone https://github.com/Matoking/NVIDIA-vBIOS-VFIO-Patcher.git

  cd NVIDIA-vBIOS-VFIO-Patcher
  #.tmp/files/git-clones/NVIDIA-vBIOS-VFIO-Patcher

  python nvidia_vbios_vfio_patcher.py -i ../../input.rom -o ../../../hooks/patched.rom || cp ../../input.rom ../../../hooks/patched.rom

  cd ../../../../
  #bash-scripts
fi
