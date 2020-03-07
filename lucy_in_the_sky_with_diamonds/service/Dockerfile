FROM ubuntu:18.04

WORKDIR /root/
RUN apt-get update && apt-get install -y \
    build-essential \
    wget \
    lib32z1

RUN useradd -u 31337 -ms /bin/bash chall
USER chall
WORKDIR /home/chall

WORKDIR /home/chall/service

CMD cd ./rw && ../ro/diamonds 6666
