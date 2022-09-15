FROM gcc
RUN apt update -y && apt-get install -y python3-pip gdb libgtk2.0-dev libva-dev libvdpau-dev clang-format libssl-dev zlib1g-dev file bsdmainutils ccache gcovr clang-tidy-13 && python3 -m pip install conan cpplint
RUN conan profile new default --detect
RUN conan profile update settings.compiler.libcxx=libstdc++11 default
RUN update-alternatives --install /usr/bin/python python /usr/bin/python3 1
RUN update-alternatives --config python

RUN curl https://github.com/Kitware/CMake/releases/download/v3.24.1/cmake-3.24.1-linux-x86_64.tar.gz -L -o cmake.tar.gz && \
    tar -vxzf cmake.tar.gz && \ 
    cp -r cmake-3.24.1-linux-x86_64 /usr/local/share/cmake 
ENV PATH=${PATH}:/usr/local/share/cmake/bin
ENV CMAKE_ROOT=/usr/local/share/cmake

RUN git clone https://github.com/rui314/mold.git && \
    cd mold && \
    git checkout v1.4.1 && \
    make -j$(nproc) && \
    make install


