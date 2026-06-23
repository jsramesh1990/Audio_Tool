# Toradex Verdin iMX8M Plus Yocto BSP Build Guide

## Environment

-   Host OS: Ubuntu 24.04
-   Docker Image: Ubuntu 22.04
-   User: SSR
-   Board: Toradex Verdin iMX8M Plus
-   BSP Branch: scarthgap-7.x.y

------------------------------------------------------------------------

## Overall Build Flow

``` text
Docker (Ubuntu 22.04)
        ↓
Create User (toxe)
        ↓
Install Dependencies
        ↓
repo init
        ↓
repo sync
        ↓
source export
        ↓
Configure MACHINE + DISTRO
        ↓
bitbake image
        ↓
Generate Images
        ↓
Flash Board
        ↓
Create Custom Layer
        ↓
Add Audio Application
```

------------------------------------------------------------------------

## Create Docker Container

``` bash
docker run -it -v ~/workspace:/work ubuntu:22.04 bash
```

------------------------------------------------------------------------

## Create User

``` bash
groupadd toxe

useradd -m -s /bin/bash -g toxe toxe

passwd toxe

usermod -aG sudo toxe
```

Switch:

``` bash
su - toxe
```

------------------------------------------------------------------------

## Install Dependencies

``` bash
sudo apt update

sudo apt install -y curl git wget vim sudo gawk diffstat unzip texinfo gcc build-essential chrpath socat cpio python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping xterm zstd lz4 file locales libssl-dev u-boot-tools device-tree-compiler bc bison flex rsync libelf-dev
```

------------------------------------------------------------------------

## Configure Locale

``` bash
sudo locale-gen en_US.UTF-8

sudo update-locale LANG=en_US.UTF-8

export LANG=en_US.UTF-8
export LANGUAGE=en_US:en
export LC_ALL=en_US.UTF-8
```

Verify:

``` bash
locale
```

------------------------------------------------------------------------

## Install repo Tool

``` bash
sudo curl https://storage.googleapis.com/git-repo-downloads/repo -o /usr/local/bin/repo

sudo chmod +x /usr/local/bin/repo

repo version
```

------------------------------------------------------------------------

## Create Yocto Workspace

``` bash
mkdir -p ~/workspace/yocto-bsp

cd ~/workspace/yocto-bsp
```

------------------------------------------------------------------------

## Initialize Toradex BSP

``` bash
repo init -u https://git.toradex.com/toradex-manifest.git -b scarthgap-7.x.y -m tdxref/default.xml
```

------------------------------------------------------------------------

## Download Sources

``` bash
repo sync -j8
```

------------------------------------------------------------------------

## Setup Environment

``` bash
source export
```

------------------------------------------------------------------------

## Configure local.conf

Edit:

``` bash
vi build/conf/local.conf
```

Important settings:

``` bash
MACHINE ?= "verdin-imx8mp"

DISTRO = "tdx-xwayland"

ACCEPT_FSL_EULA = "1"

DL_DIR ?= "${TOPDIR}/../downloads"
```

------------------------------------------------------------------------

## Verify Layers

``` bash
bitbake-layers show-layers
```

Expected layers:

-   meta-toradex-nxp
-   meta-freescale
-   meta-openembedded
-   meta-qt5
-   meta-toradex-demos
-   meta-toradex-distro
-   openembedded-core

------------------------------------------------------------------------

## Build Multimedia Image

``` bash
bitbake tdx-reference-multimedia-image
```

Build includes:

-   ATF
-   U-Boot
-   Linux Kernel
-   Device Tree
-   RootFS
-   ALSA
-   PipeWire
-   GStreamer
-   Wayland/XWayland
-   Qt
-   Docker Runtime

------------------------------------------------------------------------

## Output Images

``` bash
cd tmp/deploy/images/verdin-imx8mp

ls
```

Expected:

``` text
Image
flash.bin
*.dtb
*.wic
rootfs.tar.gz
TEZI/
```

------------------------------------------------------------------------

## Audio Product Stack

``` text
Toradex Verdin iMX8M Plus

↓

U-Boot

↓

Linux Kernel

↓

ALSA

↓

PipeWire

↓

GStreamer

↓

DSP Engine

↓

Custom Audio Application

↓

Yocto Image

↓

TEZI Image

↓

Final Product
```
