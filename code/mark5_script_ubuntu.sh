#!/bin/bash

gcc mark5_client.c -o client
gcc mark5_server.c -o server -lpthread

gnome-terminal -- bash -c "./server 127.0.0.1 8080" &

sleep 2

for ((i=1; i<=10; i++))
do
gnome-terminal -- bash -c "./client 127.0.0.1 8080" &
sleep 1
done

wait