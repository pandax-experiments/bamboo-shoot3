FROM centos:6
RUN yum -q -y install centos-release-scl && yum -q -y install "@Development Tools" devtoolset-4-toolchain cmake
ENV PATH=/opt/rh/devtoolset-4/root/usr/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin
ADD . /usr/src/bs3
CMD mkdir /tmp/build && cd /tmp/build && cmake -DCMAKE_BUILD_TYPE=Testing /usr/src/bs3 && make check
