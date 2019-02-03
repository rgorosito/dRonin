FROM ubuntu:18.04
#FROM ubuntu:latest

VOLUME /dRonin.git

WORKDIR /dRonin.git

RUN [ "apt-get", "update" ]

#RUN [ "apt-get", "install", "-y", "build-essential", "gdb" ]
RUN  apt-get install -y build-essential gdb wget debhelper ccache git \
gcc-multilib \
zlib1g-dev libusb-1.0-0-dev libudev-dev libgl1-mesa-dev libpulse-dev \
qbs curl xvfb python
#unzip
#zip para el empaquetado

RUN mkdir /root/.xdg
ENV XDG_RUNTIME_DIR=/root/.xdg

RUN xvfb-run make qt_sdk_install
RUN make breakpad_install
RUN make arm_sdk_install
RUN make all
#RUN make gtest_install
#RUN make make all_ut

