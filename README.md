# VMInstallerCLI

THIS AN EARLY ALPHA WITHOUT ALL FEATURES IMPLEMENTED YET. THERE ARE BUGS / ISSUES. I AM CURRENLY REFACTORING THIS CODE FOR BETTER SCALABILITY / MAINTAINABILITY. THIS IS A WIP WHICH DOESN'T WORK PROPERLY YET: THE FOLLOWING DOES NOT WORK YET

> - headless
> 
> - QXL / spice video
> 
> - VNC
> 
> - revert after install
> 
> - install feedback

## Introduction

VMInstallerCLI is a GUI application that is capable of setting up type-one-hypervisor VM, without the user having to write scripts, configure grub, xml edits, ...

It's goal is to make it as easy and as quick as possible to set up a VM

CURRENTLY ONLY WORKS UNDER A LIMITED AMOUNT OF CONDITIONS. I HAVE NOT ADJUSTED FOR PEOPLE THAT USE A DISPLAY MANAGER OTHER THAN GDM, SDDM, or lightDM. PASSING THROUGH INTEGRATED GRAPHICS PROCESSORS MAY NOT WORK.

### Dependencies

#### Ubuntu 22.04

```bash
sudo apt install virt-manager ovmf git meson libgtk-4-dev libadwaita-1-dev xmlstarlet qemu swtpm
```

#### Debian testing (gtk4 not in stable yet)

```bash
sudo apt install virt-manager ovmf git meson libgtk-4-dev libadwaita-1-dev xmlstarlet qemu-kvm qemu-efi swtpm
```

#### Fedora 36

```bash
sudo dnf install virt-manager edk2-ovmf git meson gtk4 libadwaita xmlstarlet qemu qemu-kvm qemu-common swtpm
```

#### Arch

```bash
sudo pacman -S virt-manager edk2-ovmf git meson gtk4 libadwaita xmlstarlet qemu-full swtpm
```

### Compilation

```bash
meson .build
ninja -C .build
./.build/VMInstallerCLI
```
