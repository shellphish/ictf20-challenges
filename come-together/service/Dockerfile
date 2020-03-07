FROM ubuntu:18.04

WORKDIR /root/
RUN apt-get update && apt-get install -y \
    build-essential \
    wget \
    imagemagick \
&& rm -rf /var/lib/apt/lists/*

RUN useradd --uid 31337 -ms /bin/bash chall
USER chall
WORKDIR /home/chall

WORKDIR /home/chall/service

CMD cd ./ro && ./wisdom.sh
