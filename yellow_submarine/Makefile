bundle:
	tar -czvf ../yellow_submarine.tgz service/

scriptbot_scripts:
	docker build -t $(SERVICE_NAME)_scripts ./scripts

clean:
	rm -rf service/rw service/append
	mkdir -p service/rw
	touch service/rw/.gitkeep
	mkdir -p service/append
	touch service/append/.gitkeep
