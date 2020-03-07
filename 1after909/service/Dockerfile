FROM ubuntu:18.04

# setup
RUN apt-get update && apt-get -y install xinetd jp2a openssl
RUN useradd -m --uid 31337 chall

# copy xinetd config and run
COPY xinetd /etc/xinetd.d/1after909
CMD ["/usr/sbin/xinetd", "-dontfork"]
