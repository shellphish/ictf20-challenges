BINPATH := service/ro/she_came_in_through_the_bathroom_window
BUNDLE_PATH := ../she_came_in_through_the_bathroom_window.tgz

service: $(BINPATH)

$(BINPATH):
	gcc -Wall -o chall src/chall.c -lcap -pie
	strip ./chall
	mkdir -p service/ro
	mv ./chall $(BINPATH)

clean:
	rm service/ro/* service/rw/* 2>/dev/null || true
	rm -rf service/append/*

scriptbot_scripts:
	docker build -t $(SERVICE_NAME)_scripts ./scripts

bundle: service
	rm $(BUNDLE_PATH) 2>/dev/null || true
	tar caf $(BUNDLE_PATH) ./docker-compose.yml ./info.yaml ./scripts/* ./service/* ./Makefile
