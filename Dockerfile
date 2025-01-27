FROM ubuntu:20.04

#LABEL maintainer="sleong@wustl.edu"

# Setup LSF link libraries
COPY lsf/ /opt/ibm/lsfsuite/lsf/10.1/

RUN export DEBIAN_FRONTEND=noninteractive && apt-get update && \
    apt-get install -y \
    autoconf \
    automake \
    autotools-dev \
    bison \
    chrpath \
    debhelper \
    dpatch \
    ethtool \
    flex \
    gfortran \
    git \
    graphviz \
    iproute2 \
    kmod \
    libelf1 \
    libfind-lib-perl \
    libgfortran4 \
    libglib2.0-0 \
    libltdl-dev \
    libmnl0 \
    libnl-3-200 \
    libnl-route-3-200 \
    libnuma1 \
    libnuma-dev \
    libssl1.1 \
    lsb-release \
    lsof \
    m4 \
    net-tools \
    pciutils \
    pkg-config \
    python \
    swig \
    tcl \
    tk \
    tk8.6 \
    tzdata \
    wget && \
    apt-get clean all

# Set MOFED version, OS version and platform
ENV MOFED_VERSION=5.8-6.0.4.2
ENV OS_VERSION=ubuntu20.04
ENV PLATFORM=x86_64

RUN wget -q http://content.mellanox.com/ofed/MLNX_OFED-${MOFED_VERSION}/MLNX_OFED_LINUX-${MOFED_VERSION}-${OS_VERSION}-${PLATFORM}.tgz && \
    tar -xvf MLNX_OFED_LINUX-${MOFED_VERSION}-${OS_VERSION}-${PLATFORM}.tgz && \
    MLNX_OFED_LINUX-${MOFED_VERSION}-${OS_VERSION}-${PLATFORM}/mlnxofedinstall \
      --user-space-only \
      --without-fw-update  \
      --with-neohost-backend \
      -q && \
    cd .. && \
    rm -rf ${MOFED_DIR} && \
    rm -rf *.tgz


ENV OPENMPI_VERSION_MAJOR=4.1
ENV OPENMPI_VERSION=4.1.7
# Install Open MPI
RUN mkdir /tmp/openmpi && \
    cd /tmp/openmpi && \
    wget -q https://download.open-mpi.org/release/open-mpi/v${OPENMPI_VERSION_MAJOR}/openmpi-${OPENMPI_VERSION}.tar.gz && \
    tar zxf openmpi-${OPENMPI_VERSION}.tar.gz && \
    cd openmpi-${OPENMPI_VERSION} && \
    ./configure \
        --with-lsf=/opt/ibm/lsfsuite/lsf/10.1/ \
        --enable-orterun-prefix-by-default \
        --with-lsf-libdir=/opt/ibm/lsfsuite/lsf/10.1/linux2.6-glibc2.3-x86_64/lib/ \
        --with-ucx=/usr --enable-mca-no-build=btl-uct && \
    make -j $(nproc) all && \
    make install && \
    ldconfig && \
    rm -rf /tmp/openmpi

