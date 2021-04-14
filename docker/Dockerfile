FROM ubuntu:focal

RUN apt update && apt install build-essential gcc-10 g++-10 python3 python3-pip -y

WORKDIR /project
COPY . /project/

RUN pip3 install cget cmake

RUN cget init --std=c++11 --shared && apt update && apt install build-essential && \
    cget install && mkdir /build && cd /build && cmake -DBUILD_SHARED_LIB=ON /project

CMD cd /build && make && make test
