all: test

server: lab3_server.c
	rm server && gcc lab3_server.c -o server && chmod +x server

client: lab3_client.c
	rm client && gcc lab3_client.c -o client && chmod +x client

test:
	./client& ./server