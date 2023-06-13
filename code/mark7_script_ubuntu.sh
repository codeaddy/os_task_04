#!/bin/bash

gcc mark7_client.c -o client
gcc mark7_observer.c -o observer
gcc mark7_server.c -o server -lpthread

gnome-terminal -- bash -c "./server 127.0.0.1 8080" &

sleep 2

gnome-terminal -- bash -c "./observer 127.0.0.1 8080" &

for ((i=1; i<=10; i++))
do
gnome-terminal -- bash -c "./client 127.0.0.1 8080" &
sleep 1
done

wait