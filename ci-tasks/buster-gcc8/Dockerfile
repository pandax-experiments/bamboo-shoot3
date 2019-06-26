FROM debian:buster-slim
RUN apt-get update && env DEBIAN_FRONTEND=noninteractive apt-get -y --no-install-recommends install g++ make cmake lcov
ADD . /usr/src/bs3
CMD mkdir /tmp/build && cd /tmp/build && cmake -DCMAKE_BUILD_TYPE=Coverage /usr/src/bs3 && make check && /usr/src/bs3/lcov-report.sh /usr/src/bs3 .
