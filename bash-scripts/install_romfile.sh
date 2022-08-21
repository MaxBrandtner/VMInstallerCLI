#! /bin/bash
# $2 = selected_gpu_number

#ls ~/.local/share/VMInstaller-CLI || mkdir ~/.local/share/VMInstaller-CLI
#ls ~/.local/share/VMInstaller-CLI/install_process || mkdir ~/.local/share/VMInstaller-CLI/install_process
#cd ~/.local/share/VMInstaller-CLI/install_process

if [$(lspci -nn | grep '\[0300' | head -n $2| tail -n 1| grep NVIDIA) == 0]
then
  git clone https://github.com/Matoking/NVIDIA-vBIOS-VFIO-Patcher.git

  cd NVIDIA-vBIOS-VFIO-Patcher

  python nvidia_vbios_vfio_patcher.py -i files/input.rom -o ../hooks/patched.rom
else
  mv $1 ../hooks/patched.rom
fi
