FROM ubuntu:17.10

WORKDIR /build
ADD . /build

RUN apt-add-repository "deb http://apt.llvm.org/artful/ llvm-toolchain-artful-6.0 main"
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|sudo apt-key add -
RUN apt update
RUN apt -y install build-essential clang-6.0 llvm-6.0 cmake-data cmake libboost-all-dev ninja

CMD ["/bin/bash", "build.sh"]