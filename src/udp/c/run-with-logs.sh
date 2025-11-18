#!/usr/bin/env bash

cleanup() {
    echo ""
    echo "Stopping containers and cleaning up"

    kill $(jobs -p) 2>/dev/null

    docker-compose -f docker-compose.c.yml down

    echo "Cleanup complete!"
}

trap cleanup EXIT INT TERM

docker-compose -f docker-compose.c.yml up --build -d

docker-compose -f docker-compose.c.yml logs -f cserver > cserver.log 2>&1 &
docker-compose -f docker-compose.c.yml logs -f cclient > cclient.log 2>&1 &

echo "Containers started. Logs being written to cserver.log and cclient.log"
echo "Press Ctrl+C to stop containers and exit"

wait