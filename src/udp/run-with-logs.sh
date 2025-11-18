#!/usr/bin/env bash

COMPOSE_FILE="docker-compose.yml"

LOGS=(
    cserver_to_c
    cserver_to_p
    pserver_to_c
    pserver_to_p
    cclient_to_c
    cclient_to_p
    pclient_to_c
    pclient_to_p
)

cleanup() {
    echo ""
    echo "Stopping containers and cleaning up"

    kill $(jobs -p) 2>/dev/null

    docker compose -f "$COMPOSE_FILE" down

    echo "Cleanup complete!"
}

trap cleanup EXIT INT TERM

echo "Starting containers..."
docker compose -f "$COMPOSE_FILE" up --build -d

for svc in "${LOGS[@]}"; do
    echo "Streaming logs for $svc → ${svc}.log"
    docker compose --ansi -f "$COMPOSE_FILE" logs -f "$svc" > "${svc}.log" 2>&1 &
done

echo ""
echo "Containers started."
echo "Logs are being written to:"
for svc in "${LOGS[@]}"; do
    echo "  - ${svc}.log"
done

echo ""
echo "Press Ctrl+C to stop containers and exit."
wait
