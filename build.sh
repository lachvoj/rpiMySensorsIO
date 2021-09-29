#!/bin/bash

git clone https://github.com/mysensors/MySensors.git
cd MySensors
git checkout b9d9cc339659f724aa94d4108fc5289a720d1bcd

#rpi
./configure --soc=BCM2835 --my-transport=none --my-debug=enable --my-gateway=ethernet --my-port=5003 --extra-cxxflags="-DLINUX_ARCH_RASPBERRYPI" --extra-cflags="-DLINUX_ARCH_RASPBERRYPI"

#rpi4
./configure --soc=BCM2711 --my-transport=none --my-debug=enable --my-gateway=ethernet --my-port=5003 --extra-cxxflags="-DLINUX_ARCH_RASPBERRYPI" --extra-cflags="-DLINUX_ARCH_RASPBERRYPI"

#pc
./configure --my-transport=none --my-debug=enable --my-gateway=ethernet --my-port=5003 --extra-cxxflags="-DHAVE_STROPTS_H=0" --extra-cflags="-DHAVE_STROPTS_H=0"
