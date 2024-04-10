1. Find a open port
2. Allow user to download a model
3. Add checksums

Add macdeployqt

Add models
https://huggingface.co/mistralai/Mistral-7B-Instruct-v0.1
https://huggingface.co/stabilityai/stable-diffusion-xl-base-1.0
runpod integration

sudo apt-get install libxcb1-dev libxcb-util0-dev libxcb-cursor-dev libxcb-xv0-dev

sudo apt-get install '^libxcb.*-dev' libx11-xcb-dev libglu1-mesa-dev libxrender-dev libxi-dev libxkbcommon-dev libxkbcommon-x11-dev

Things to do between now and mac launch


Today
1. Build where Python framework is in the executable
2. Figure out where source code will live and write a simple fastapi server
3. Write a simple fastapi server and launch that from a button
4. 
1. Add legal page


curl --request POST \
--url http://localhost:8080/completion \
--header "Content-Type: application/json" \
--data '{"prompt": "Building a website can be done in 10 simple steps:","n_predict": 128}'

curl http://localhost:8080/v1/embeddings \
-H "Content-Type: application/json" \
-d '{"input": "Deep is a good boy"}'



sudo apt-get install bison build-essential flex gperf libasound2-dev libatkmm-1.6-dev libbz2-dev libcap-dev libcups2-dev libdrm-dev libegl1-mesa-dev libfontconfig1-dev libfreetype6-dev libglu1-mesa-dev libicu-dev libnss3-dev libpci-dev libpulse-dev libssl-dev libudev-dev libx11-dev libx11-xcb-dev libxcb-composite0 libxcb-composite0-dev libxcb-cursor-dev libxcb-cursor0 libxcb-damage0 libxcb-damage0-dev libxcb-dpms0 libxcb-dpms0-dev libxcb-dri2-0 libxcb-dri2-0-dev libxcb-dri3-0 libxcb-dri3-dev libxcb-ewmh-dev libxcb-ewmh2 libxcb-glx0 libxcb-glx0-dev libxcb-icccm4 libxcb-icccm4-dev libxcb-image0 libxcb-image0-dev libxcb-keysyms1 libxcb-keysyms1-dev libxcb-present-dev libxcb-present0 libxcb-randr0 libxcb-randr0-dev libxcb-record0 libxcb-record0-dev libxcb-render-util0 libxcb-render-util0-dev libxcb-render0 libxcb-render0-dev libxcb-res0 libxcb-res0-dev libxcb-screensaver0 libxcb-screensaver0-dev libxcb-shape0 libxcb-shape0-dev libxcb-shm0 libxcb-shm0-dev libxcb-sync-dev libxcb-sync0-dev libxcb-sync1 libxcb-util-dev libxcb-util0-dev libxcb-util1 libxcb-xf86dri0 libxcb-xf86dri0-dev libxcb-xfixes0 libxcb-xfixes0-dev libxcb-xinerama0 libxcb-xinerama0-dev libxcb-xkb-dev libxcb-xkb1 libxcb-xtest0 libxcb-xtest0-dev libxcb-xv0 libxcb-xv0-dev libxcb-xvmc0 libxcb-xvmc0-dev libxcb1 libxcb1-dev libxcomposite-dev libxcursor-dev libxdamage-dev libxext-dev libxfixes-dev libxi-dev libxrandr-dev libxrender-dev libxslt-dev libxss-dev libxtst-dev perl

sudo apt install adduser alsa-ucm-conf apt-listchanges base-files base-passwd bash-completion bash bind9-dnsutils bind9-host bison bsdutils build-essential busybox bzip2 ca-certificates clang cmake console-setup coreutils cpio cron-daemon-common cron curl dash dbus debconf-i18n debconf debhelper debian-archive-keyring debian-faq debianutils devscripts diffutils discover dmidecode dmsetup doc-debian dpkg e2fsprogs emacs-nox fakeroot fdisk file findutils flatpak flex gcc-12-base gcr gettext-base gir1.2-gck-1 gir1.2-gcr-3 gir1.2-secret-1 git gpgv grep groff-base grub-common grub-efi-amd64 gzip hostname htop ifupdown inetutils-telnet init-system-helpers init initramfs-tools installation-report iotop iproute2 iputils-ping isc-dhcp-client isc-dhcp-common keyboard-configuration kmod krb5-locales laptop-detect less libacl1 libapparmor1 libapt-pkg6.0 libargon2-1 libattr1 libaudit-common libaudit1 libblkid1 libbpf1 libbsd0 libbz2-1.0 libc-bin libc6 libcap-ng0 libcap2-bin libcap2 libcom-err2 libcrypt1 libcryptsetup12 libdb5.3 libdebconfclient0 libdevmapper1.02.1 libdrm-dev libedit2 libelf1 libext2fs2 libfdisk1 libffi8 libfontconfig1-dev libgcc-s1 libgck-1-0 libgck-2-2 libgcr-4-4 libgcr-base-3-1 libgcr-ui-3-1 libgcrypt20 libgmp10 libgnutls30 libgpg-error0 libgssapi-krb5-2 libhogweed6 libicu-dev libicu72 libidn2-0 libip4tc2 libjansson4 libjson-c5 libk5crypto3 libkeyutils1 libkmod2 libkrb5-3 libkrb5support0 libksba8 liblocale-gettext-perl liblockfile-bin liblz4-1 liblzma5 libmd0 libmnl0 libmount1 libncursesw6 libnettle8 libnewt0.52 libnftables1 libnftnl11 libnss-myhostname libnss-systemd libp11-kit0 libpam-modules-bin libpam-modules libpam-runtime libpam-systemd libpam0g libpcre2-8-0 libplacebo338 libpopt0 libproc2-0 libreadline8 libseccomp2 libsecret-1-0 libsecret-common libselinux1 libsemanage-common libsemanage2 libsepol2 libslang2 libsmartcols1 libss2 libssl3 libstdc++6 libsystemd-shared libsystemd0 libtasn1-6 libtext-charwidth-perl libtext-iconv-perl libtext-wrapi18n-perl libtinfo6 libtirpc-common libtirpc3 libu2f-udev libudev1 libunistring2 libuuid1 libvulkan-dev libwayland-dev libx11-dev libx11-xcb-dev libxcb-composite0-dev libxcb-cursor-dev libxcb-damage0-dev libxcb-dpms0-dev libxcb-dri2-0-dev libxcb-dri3-dev libxcb-ewmh-dev libxcb-glx0-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-imdkit-dev libxcb-keysyms1-dev libxcb-present-dev libxcb-randr0-dev libxcb-record0-dev libxcb-render-util0-dev libxcb-render0-dev libxcb-res0-dev libxcb-screensaver0-dev libxcb-shape0-dev libxcb-shm0-dev libxcb-sync-dev libxcb-util-dev libxcb-util0-dev libxcb-xf86dri0-dev libxcb-xfixes0-dev libxcb-xinerama0-dev libxcb-xinput-dev libxcb-xkb-dev libxcb-xrm-dev libxcb-xtest0-dev libxcb-xv0-dev libxcb-xvmc0-dev libxcb1-dev libxfixes-dev libxrandr-dev libxshmfence-dev libxtables12 libxxhash0 libzstd1 locales login logrotate logsave lshw lsof man-db manpages mawk media-types mesa-utils meson mime-support mold mount mythes-en-us nano ncurses-base ncurses-bin ncurses-term neofetch net-tools netbase netcat-traditional ninja-build openssh-client openssh-server os-prober passwd pciutils perl-base perl popularity-contest procps psmisc python3-dev python3-mako python3-pysimplesoap python3-reportbug python3-venv readline-common reportbug sed sensible-utils shim-signed systemd-dev systemd-sysv systemd-timesyncd systemd sysvinit-utils tar task-english task-gnome-desktop task-ssh-server tasksel-data tasksel tmux traceroute tzdata ucf udev usbutils usr-is-merged util-linux-extra util-linux vim-common vim-tiny vulkan-tools wamerican wget whiptail x11proto-dri2-dev xz-utils zlib1g zstd
