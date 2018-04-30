FROM ubuntu:17.10

WORKDIR /build
ADD . /build

RUN apt update
RUN apt -y install python-software-properties add-apt-key apt perl wget 
RUN echo "deb http://apt.llvm.org/artful/ llvm-toolchain-artful-6.0 main" >> /etc/apt/sources.list
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|apt-key add -
RUN apt update
RUN apt -y install build-essential clang-6.0 llvm-6.0 cmake-data cmake libboost-all-dev

CMD ["/bin/bash", "build.sh"]
