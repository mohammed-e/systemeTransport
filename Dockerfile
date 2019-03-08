FROM ubuntu:16.04
MAINTAINER mohammed-elk

# update and install dependencies
RUN apt-get update \
    && apt-get install -y \
        software-properties-common \
        wget \
    && add-apt-repository -y ppa:ubuntu-toolchain-r/test \
    && apt-get update \
    && apt-get install -y \
        gcc \
        g++ \
        make \
        libpthread-stubs0-dev \
        vim \
        vim-gnome \
    && mkdir /systemBusSubTaxi

# copying source code
COPY ./src /systemBusSubTaxi
