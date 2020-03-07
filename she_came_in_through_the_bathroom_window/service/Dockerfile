from ubuntu:18.04

run apt-get update
run apt-get install xinetd libcap2 libcap2-bin -y
run useradd -m chall -u 31337

# give challenge chroot capability
run mkdir -p /home/chall/service/bin && chmod 555 /home/chall/service/bin
add bin/she_came_in_through_the_bathroom_window /home/chall/service/bin/she_came_in_through_the_bathroom_window
run chown root:chall /home/chall/service/bin/she_came_in_through_the_bathroom_window && chmod 550 /home/chall/service/bin/she_came_in_through_the_bathroom_window
run setcap cap_sys_chroot+eip /home/chall/service/bin/she_came_in_through_the_bathroom_window

# challenge specific config
run mkdir /tmp/bath_window /tmp/jail /tmp/room /tmp/garden
run chown chall:chall /tmp/bath_window /tmp/jail /tmp/room /tmp/garden
run echo "1337" > /tmp/bath_window/height

copy xinetd /etc/xinetd.d/she_came_in_through_the_bathroom_window
cmd ["/usr/sbin/xinetd", "-dontfork"]
