# VMInstallerCLI

THIS AN EARLY ALPHA WITHOUT ALL FEATURES IMPLEMENTED YET. THERE ARE BUGS(AT LEAST WITHIN THE UI). THE FOLLOWING DOES NOT WORK YET

> - headless
> 
> - QXC / spice video server
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

> qemu xmlstarlet virt-manager git

### Compilation

```bash
meson .build
ninja -C .build
./.build/VMInstallerCLI
```
