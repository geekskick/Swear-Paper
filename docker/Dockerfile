FROM gcc
RUN apt update -y && apt-get install -y python3-pip cmake libgtk2.0-dev libva-dev libvdpau-dev  && python3 -m pip install conan
RUN conan profile new default --detect
RUN conan profile update settings.compiler.libcxx=libstdc++11 default
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 1
RUN update-alternatives --config python
