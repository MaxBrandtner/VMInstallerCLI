#! /bin/bash
# $1 = selected_gpu_number

#ls ~/.local/share/VMInstaller-CLI || mkdir ~/.local/share/VMInstaller-CLI
#ls ~/.local/share/VMInstaller-CLI/install_process || mkdir ~/.local/share/VMInstaller-CLI/install_process
#cd ~/.local/share/VMInstaller-CLI/install_process

mkdir -p tmp
mkdir -p tmp/hooks

function try()
{
    [[ $- = *e* ]]; SAVED_OPT_E=$?
    set +e
}

function throw()
{
    exit $1
}

function catch()
{
    export ex_code=$?
    (( $SAVED_OPT_E )) && set +e
    return $ex_code
}



mkdir -p files
cd files
mkdir -p git-clones
cd git-clones

lspci -nn | grep '\[0300' | head -n $1 | tail -n 1 | grep NVIDIA
return_value=$?
if [ $return_value -eq 0 ]
then
  tree 'NVIDIA-vBIOS-VFIO-Patcher' ||git clone https://github.com/Matoking/NVIDIA-vBIOS-VFIO-Patcher.git

  cd NVIDIA-vBIOS-VFIO-Patcher

  try
  (
  python nvidia_vbios_vfio_patcher.py -i ../../input.rom -o ../../../tmp/hooks/patched.rom

  ) 
  catch || {
		cp ../../input.rom ../../../tmp/hooks/patched.rom
	}
  cd ../../..
else
  cp $1 ../../tmp/hooks/patched.rom
fi
