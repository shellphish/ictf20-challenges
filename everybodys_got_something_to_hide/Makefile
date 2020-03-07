BINDIR := ./service/append

service: $(BINDIR)/everybodys_got_something_to_hide

$(BINDIR)/everybodys_got_something_to_hide: ./src/everybodys_got_something_to_hide.c
	gcc ./src/everybodys_got_something_to_hide.c -o service/ro/everybodys_got_something_to_hide
	strip service/ro/everybodys_got_something_to_hide

clean:
	$(RM) ./service/ro/*
	$(RM) ./service/rw/*
	$(RM) -r ./service/append/*

bundle: service
	$(RM) ../everybodys_got_something_to_hide.tgz
	tar caf ../everybodys_got_something_to_hide.tgz ./docker-compose.yml ./info.yaml ./scripts/* ./service/* ./src/* ./Makefile
	@echo "#### Double check ../everybodys_got_something_to_hide.tgz and submit it :) ####"

scriptbot_scripts:
	docker build -t $(SERVICE_NAME)_scripts ./scripts