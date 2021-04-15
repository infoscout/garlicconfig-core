FROM ubuntu:focal

RUN apt update && apt install build-essential gcc-10 g++-10 python3 python3-pip -y

WORKDIR /project
COPY . /project/

RUN pip3 install cget cmake

RUN cget init -p /cget --std=c++11 --shared && apt update && apt install build-essential && \
    cget install -p /cget && mkdir /build && \
    cd /build && cmake -DBUILD_SHARED_LIB=ON -DCGET_PATH=/cget/cget/cget.cmake /project

CMD cd /build && make && make test
