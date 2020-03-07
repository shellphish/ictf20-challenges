FROM ubuntu:18.04

RUN apt-get update \
 && apt-get install -y libc6-dev-i386 python python-pip build-essential socat \
 && pip install nclib \
 && useradd -ms /bin/bash --uid 31337 chall

USER chall
WORKDIR /home/chall

CMD cd service/append && socat -v tcp-l:6666,reuseaddr,fork exec:"python -u ../ro/main.py"