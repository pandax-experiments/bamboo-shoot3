FROM buildpack-deps:jessie
RUN apt-get update && env DEBIAN_FRONTEND=noninteractive apt-get -y --no-install-recommends install clang cmake
ENV CC=clang CXX=clang++
ADD . /usr/src/bs3
CMD mkdir /tmp/build && cd /tmp/build && cmake -DCMAKE_BUILD_TYPE=Testing /usr/src/bs3 && make check
