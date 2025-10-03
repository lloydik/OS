all: test

server: server.c
	rm server && gcc server.c -o server && chmod +x server

client: client.c
	rm client && gcc client.c -o client && chmod +x client

test:
	./client& ./server