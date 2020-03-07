all: come_together setupsession parsepeace setupimage domagic

clean:
	@rm -rf *.o
	@rm -rf come_together
	@rm -rf setupsession

come_together: main.o httpd.o utils.o session.o
	gcc -o come_together $^

main.o: main.c httpd.h utils.h session.h
	gcc -c -o main.o main.c

httpd.o: httpd.c httpd.h
	gcc -c -o httpd.o httpd.c

utils.o: utils.c utils.h
	gcc -c -o utils.o utils.c

session.o: session.c session.h
	gcc -c -o session.o session.c

setupsession: setupsession.c
	gcc -o setupsession setupsession.c

parsepeace: parsepeace.c
	gcc -o parsepeace parsepeace.c

setupimage: setupimage.c
	gcc -o setupimage setupimage.c

domagic: domagic.c
	gcc -o domagic domagic.c
