all: test

server: server.c
	gcc server.c -o server && chmod +x server

client: client.c
	gcc client.c -o client && chmod +x client

test:
	./client& ./server