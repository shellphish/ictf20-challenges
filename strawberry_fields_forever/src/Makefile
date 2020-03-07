bundle:
	gcc -O0 -Wall -pie -fPIE strawberry_fields.c -o strawberry_fields
	strip -s strawberry_fields
	cp strawberry_fields ../service/ro
clean:
	rm strawberry_fields
