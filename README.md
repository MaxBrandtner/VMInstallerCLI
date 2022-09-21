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

WORKAROUND FOR LACK OF OS SELECTION
```bash
export VIRTINSTALL_OSINFO_DISABLE_REQUIRE=1
```


## Introduction

VMInstallerCLI is a GUI application that is capable of setting up type-one-hypervisor VM, without the user having to write scripts, configure grub, xml edits, ...

It's goal is to make it as easy and as quick as possible to set up a VM

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
