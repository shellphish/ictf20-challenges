.PHONY: all service bundle spawn
SERVICE=1after909
PORT=1909
SOURCES=src/1after909.c
all: bundle
1after909: ${SOURCES}
	gcc -O0 -Wall -Wno-unused-variable ${SOURCES} -o src/${SERVICE} -lcrypto

service: 1after909
	mkdir -p service/ro
	mkdir -p service/rw
	cp src/$(SERVICE) service/ro
	cp src/run.sh service/ro
	chmod +x service/ro/run.sh
	mkdir -p service/rw

bundle: service
	rm -f ../$(SERVICE).tgz
	tar -czvf ../$(SERVICE).tgz --exclude 1after909_env *
	@echo "#### Double check ../$(SERVICE).tgz and submit it :) ####"

spawn: service
	( cd service/rw && socat tcp-l:$(PORT),reuseaddr,fork exec:"../ro/$(SERVICE)" 2>>error.log )

clean:
	rm -fr service/ro
	rm -rf service/rw

scriptbot_scripts:
	docker build -t $(SERVICE_NAME)_scripts ./scripts
