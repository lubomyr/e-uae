#!/bin/sh


LOCAL_PATH=`dirname $0`
LOCAL_PATH=`cd $LOCAL_PATH && pwd`

ln -sf libsdl-1.2.so $LOCAL_PATH/../../../obj/local/armeabi-v7a/libSDL.so
ln -sf libsdl-1.2.so $LOCAL_PATH/../../../obj/local/armeabi-v7a/libpthread.so
ln -sf libsdl_image.so $LOCAL_PATH/../../../obj/local/armeabi-v7a/libSDL_image.so
ln -sf libsdl_ttf.so $LOCAL_PATH/../../../obj/local/armeabi-v7a/libSDL_ttf.so
ln -sf libguichan.so $LOCAL_PATH/../../../obj/local/armeabi-v7a/libguichan_sdl.so


if [ "$1" = armeabi-v7a ]; then
if [ \! -f euae/configure ] ; then
	sh -c "cd euae && ./bootstrap.sh"
fi

if [ \! -f euae/Makefile ] ; then
env CFLAGS="-Ofast -marm -ffast-math" \
env CXXFLAGS="-marm" \
#	../setEnvironment-armeabi-v7a.sh sh -c "cd euae && ./configure --build=x86_64-unknown-linux-gnu --host=arm-linux-androideabi --with-sdl --with-sdl-sound --with-sdl-gfx --without-x --disable-ui --without-zlib"
	../setEnvironment-armeabi-v7a.sh sh -c "cd euae && ./configure --build=x86_64-unknown-linux-gnu --host=arm-linux-androideabi --with-sdl --with-sdl-sound --with-sdl-gfx --without-x --disable-gtktest --without-zlib --with-guichan-gui"
fi

make -C euae && mv -f euae/src/uae libapplication-armeabi-v7a.so
fi

#if [ "$1" = x86 ]; then
#if [ \! -f euae-x86/configure ] ; then
#	sh -c "cd euae-x86 && ./bootstrap.sh"
#fi
#
#if [ \! -f euae-x86/Makefile ] ; then
#	../setEnvironment-x86.sh sh -c "cd euae-x86 && ./configure --build=x86_64-unknown-linux-gnu --host=i686-linux-android --with-sdl --with-sdl-sound --with-sdl-gfx --without-x --disable-ui"
#fi
#
#make -C euae-x86 && mv -f euae-x86/src/uae libapplication-x86.so
#fi