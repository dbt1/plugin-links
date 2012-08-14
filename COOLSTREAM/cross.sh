PREFIX=/opt/newcross/arm-cx2450x-linux-gnueabi/arm-cx2450x-linux-gnueabi/sys-root/usr
export PKG_CONFIG_PATH=${PREFIX}/lib/pkgconfig
CC=arm-cx2450x-linux-gnueabi-gcc ./configure -host=arm-cx2450x-linux-gnueabi -prefix=/opt/newcross/arm-cx2450x-linux-gnueabi/arm-cx2450x-linux-gnueabi/sys-root/usr --without-svgalib --without-x --without-directfb --without-libtiff --enable-graphics
