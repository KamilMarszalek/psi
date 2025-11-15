#!/usr/bin/env bash

if [ "$#" -lt 1 ]; then
    echo "Usage: $0 <delay_ms> [jitter_ms] [loss_%]"
    echo "Example: $0 1000 500 50"
    echo "  - delay: base delay in milliseconds (required)"
    echo "  - jitter: delay variation in milliseconds (optional, default: 0)"
    echo "  - loss: packet loss percentage (optional, default: 0)"
    exit 1
fi

DELAY="${1}ms"
JITTER="${2:-0}ms"
LOSS="${3:-0}%"

echo "Network configuration:"
echo "  Delay: $DELAY"
echo "  Jitter: $JITTER"
echo "  Loss: $LOSS"
echo ""

cleanup() {
    echo ""
    echo "Stopping containers and cleaning up"

    kill $(jobs -p) 2>/dev/null

    docker-compose -f docker-compose.c.yml down

    echo "Cleanup complete!"
}

trap cleanup EXIT INT TERM

echo "Building containers"
docker-compose -f docker-compose.c.yml build

sleep 1

echo "Applying netem to cserver"
docker exec z11_cserver_udp tc qdisc add dev eth0 root netem delay $DELAY $JITTER loss $LOSS

echo "Starting containers"
docker-compose -f docker-compose.c.yml up -d

echo ""
echo "Network conditions applied!"
echo ""

docker-compose -f docker-compose.c.yml logs -f cserver > cserver.log 2>&1 &
docker-compose -f docker-compose.c.yml logs -f cclient > cclient.log 2>&1 &

echo "Containers started. Logs being written to cserver.log and cclient.log"
echo "Press Ctrl+C to stop containers and exit"

wait
