all: test

server: lab3_server.c
	gcc lab3_server.c -o server && chmod +x server

client: lab3_client.c
	gcc lab3_client.c -o client && chmod +x client

test:
	./client& ./server