FROM gcc:4.9
RUN apt-get update && env DEBIAN_FRONTEND=noninteractive apt-get -y --no-install-recommends install cmake
ADD . /usr/src/bs3
CMD mkdir /tmp/build && cd /tmp/build && cmake -DCMAKE_BUILD_TYPE=Testing /usr/src/bs3 && make check
