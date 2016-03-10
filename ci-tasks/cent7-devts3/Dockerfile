FROM centos:7
RUN yum -q -y install centos-release-scl && yum -qy install "@Development Tools" devtoolset-3-toolchain cmake
ENV PATH=/opt/rh/devtoolset-3/root/usr/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
ADD . /usr/src/bs3
CMD mkdir /tmp/build && cd /tmp/build && cmake -DCMAKE_BUILD_TYPE=Testing /usr/src/bs3 && make check
