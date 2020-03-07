FROM ubuntu:18.04

# setup
RUN apt-get update && apt-get -y install xinetd
RUN useradd -m --uid 31337 chall

# copy xinetd config and run
COPY xinetd /etc/xinetd.d/everybodys_got_something_to_hide
CMD ["/usr/sbin/xinetd", "-dontfork"]
