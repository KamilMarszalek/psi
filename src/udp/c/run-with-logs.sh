#!/usr/bin/env bash

docker-compose -f docker-compose.c.yml up --build -d

docker-compose -f docker-compose.c.yml logs -f cserver > cserver.log 2>&1 &
docker-compose -f docker-compose.c.yml logs -f cclient > cclient.log 2>&1 &

echo "Containers started. Logs being written to cserver.log and cclient.log"
echo "Press Ctrl+C to stop logging (containers will keep running)"

wait
