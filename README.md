# VMInstallerCLI

THIS AN EARLY ALPHA WITHOUT ALL FEATURES IMPLEMENTED YET. THERE ARE BUGS / ISSUES. THIS IS A WIP: THE FOLLOWING DOES NOT WORK YET

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

### Dependencies

#### ubuntu 22.04
```bash
sudo apt install virt-manager ovmf git meson libgtk-4-dev libadwaita-1-dev sed grep xmlstarlet qemu swtpm
```

#### Debian testing (gtk4 not in stable yet)
```bash
sudo apt install virt-manager ovmf git meson libgtk-4-dev libadwaita-1-dev sed grep xmlstarlet qemu-kvm qemu-efi swtpm
```

#### Fedora 36
```bash
sudo dnf install virt-manager edk2-ovmf git meson gtk4 libadwaita sed grep xmlstarlet qemu qemu-kvm qemu-common swtpm
```

#### Arch
```bash
sudo pacman -S virt-manager edk2-ovmf git meson gtk4 libadwaita sed grep xmlstarlet qemu-full swtpm
```



### Compilation

```bash
meson .build
ninja -C .build
./.build/VMInstallerCLI
```
