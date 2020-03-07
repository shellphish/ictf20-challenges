from ubuntu:18.04

run apt-get update && apt-get install -y xinetd libcap2 libcap2-bin
run apt-get update && apt-get install -y seccomp libseccomp-dev
run apt-get update && apt-get install -y netcat-openbsd strace
run useradd --uid 31337 -m chall

copy ./xinetd /etc/xinetd.d/strawberry_fields
RUN chmod 644 /etc/xinetd.d/strawberry_fields
USER chall
cmd ["/usr/sbin/xinetd", "-dontfork"]
