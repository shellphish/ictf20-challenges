bundle:
	#gcc -O0 -Wall -pie -fPIE src/strawberry_fields.c -o src/strawberry_fields
	#strip -s src/strawberry_fields
	cp src/strawberry_fields service/ro
	tar czvf ../strawberry_fields_forever.tgz service/

scriptbot_scripts:
	docker build -t $(SERVICE_NAME)_scripts ./scripts

clean:
	rm -f service/ro/strawberry_fields