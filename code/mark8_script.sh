#!/bin/bash

gcc mark7_client.c -o client
gcc mark7_observer.c -o observer
gcc mark7_server.c -o server -lpthread

osascript -e "tell application \"Terminal\" to do script \"$(pwd)/server 127.0.0.1 8080\"" &

sleep 2

osascript -e "tell application \"Terminal\" to do script \"$(pwd)/observer 127.0.0.1 8080\"" &
osascript -e "tell application \"Terminal\" to do script \"$(pwd)/observer 127.0.0.1 8080\"" &
osascript -e "tell application \"Terminal\" to do script \"$(pwd)/observer 127.0.0.1 8080\"" &

for i in {1..7}
do
   osascript -e "tell application \"Terminal\" to do script \"$(pwd)/client 127.0.0.1 8080\"" &
   sleep 1
done

wait