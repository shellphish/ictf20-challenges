bundle:
	$(MAKE) -C src
	mkdir service/ro
	mkdir service/rw
	cp src/come_together service/ro/
	cp src/setupsession service/ro/
	cp src/parsepeace service/ro/
	cp src/setupimage service/ro/
	cp src/domagic service/ro/
	cp src/wisdom.sh service/ro/
	cp -r src/images service/ro/
	cp -r src/templates service/ro/
	ln -s ../rw/data service/ro/data
	chmod +x service/ro/come_together
	chmod +x service/ro/setupsession
	chmod +x service/ro/parsepeace
	chmod +x service/ro/setupimage
	chmod +x service/ro/domagic
	chmod +x service/ro/wisdom.sh
	tar czvf ../come-together.tgz service/
	echo "Ready to fire docker!"

scriptbot_scripts:
	docker build -t $(SERVICE_NAME)_scripts ./scripts

clean:
	rm -f service/ro/come_together
	rm -f service/ro/setupsession
	rm -f service/ro/parsepeace
	rm -f service/ro/setupimage
	rm -f service/ro/domagic
	rm -f service/ro/wisdom.sh
	rm -f service/ro/data
	rm -rf service/ro
	rm -rf service/rw
