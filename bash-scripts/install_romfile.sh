#! /bin/bash
# $1 = input romfile

#ls ~/.local/share/VMInstaller-CLI || mkdir ~/.local/share/VMInstaller-CLI
#ls ~/.local/share/VMInstaller-CLI/install_process || mkdir ~/.local/share/VMInstaller-CLI/install_process
#cd ~/.local/share/VMInstaller-CLI/install_process

git clone https://github.com/Matoking/NVIDIA-vBIOS-VFIO-Patcher.git

cd NVIDIA-vBIOS-VFIO-Patcher

python nvidia_vbios_vfio_patcher.py -i $1 -o ../hooks/patched.rom
