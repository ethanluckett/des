all: client server
client: client.c des.c
	gcc client.c des.c -o client
server: server.c des.c
	gcc server.c des.c -o server
