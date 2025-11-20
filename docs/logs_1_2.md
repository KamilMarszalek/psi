<!--
---
title: "PSI - Zadanie 1.2 Logi"
author:
  - "**Damian D'Souza** (lider zespołu)"
  - "Kamil Marszałek"
  - "Michał Szwejk"
date: "2025-11-18"
subtitle: "Zespół Z11"
version: "1.0"
---
-->

# Zadanie 1.2

Do uruchomiena został użyty skrypt `run-with-netem.sh`, buduje on kontenery, ustawia symulację opóźnień w sieci i zapisuje logi z obu kontenerów do plików

```bash
run-with-netem.sh 1000 500 50 # przykładowe użycie, argumenty kolejno delay jitter loss
```

### Logi serwera

```
z11_cserver_udp  | Applying netem: delay=1000ms jitter=500ms loss=50%
z11_cserver_udp  | UDP server listening on port 8080...
z11_cserver_udp  |
z11_cserver_udp  | Received datagram (seq_bit=0, packet_num=1): "ABCDEFGH..."
z11_cserver_udp  |
z11_cserver_udp  | Sent ACK with seq_bit=0 for packet_num=1
z11_cserver_udp  |
z11_cserver_udp  | Received datagram (seq_bit=0, packet_num=1): "ABCDEFGH..."
z11_cserver_udp  |
z11_cserver_udp  | Sent ACK with seq_bit=0 for packet_num=1
z11_cserver_udp  |
z11_cserver_udp  | Received datagram (seq_bit=1, packet_num=2): "ABCDEFGH..."
z11_cserver_udp  |
z11_cserver_udp  | Sent ACK with seq_bit=1 for packet_num=2
z11_cserver_udp  |
z11_cserver_udp  | Received datagram (seq_bit=1, packet_num=2): "ABCDEFGH..."
z11_cserver_udp  |
z11_cserver_udp  | Sent ACK with seq_bit=1 for packet_num=2
z11_cserver_udp  |
z11_cserver_udp  | Received datagram (seq_bit=1, packet_num=2): "ABCDEFGH..."
z11_cserver_udp  |
z11_cserver_udp  | Sent ACK with seq_bit=1 for packet_num=2
z11_cserver_udp  |
z11_cserver_udp  | Received datagram (seq_bit=1, packet_num=2): "ABCDEFGH..."
z11_cserver_udp  |
z11_cserver_udp  | Sent ACK with seq_bit=1 for packet_num=2
z11_cserver_udp  |
z11_cserver_udp  | Received datagram (seq_bit=0, packet_num=3): "ABCDEFGH..."
z11_cserver_udp  |
z11_cserver_udp  | Sent ACK with seq_bit=0 for packet_num=3
...
z11_cserver_udp  | Received datagram (seq_bit=0, packet_num=27): "ABCDEFGH..."
z11_cserver_udp  |
z11_cserver_udp  | Sent ACK with seq_bit=0 for packet_num=27
z11_cserver_udp  |
z11_cserver_udp  | Received datagram (seq_bit=1, packet_num=28): "ABCDEFGH..."
z11_cserver_udp  |
z11_cserver_udp  | Sent ACK with seq_bit=1 for packet_num=28
z11_cserver_udp  |
z11_cserver_udp  | Received datagram (seq_bit=1, packet_num=28): "ABCDEFGH..."
z11_cserver_udp  |
z11_cserver_udp  | Sent ACK with seq_bit=1 for packet_num=28
z11_cserver_udp  |
z11_cserver_udp  | Received datagram (seq_bit=1, packet_num=28): "ABCDEFGH..."
z11_cserver_udp  |
z11_cserver_udp  | Sent ACK with seq_bit=1 for packet_num=28
z11_cserver_udp  |
z11_cserver_udp  | Received datagram (seq_bit=0, packet_num=29): "ABCDEFGH..."
z11_cserver_udp  |
z11_cserver_udp  | Sent ACK with seq_bit=0 for packet_num=29
z11_cserver_udp  |
z11_cserver_udp  | Received datagram (seq_bit=0, packet_num=29): "ABCDEFGH..."
z11_cserver_udp  |
z11_cserver_udp  | Sent ACK with seq_bit=0 for packet_num=29
z11_cserver_udp  |
z11_cserver_udp  | Received datagram (seq_bit=1, packet_num=30): "ABCDEFGH..."
z11_cserver_udp  |
z11_cserver_udp  | Sent ACK with seq_bit=1 for packet_num=30
z11_cserver_udp  |
z11_cserver_udp  | Received datagram (seq_bit=1, packet_num=30): "ABCDEFGH..."
z11_cserver_udp  |
z11_cserver_udp  | Sent ACK with seq_bit=1 for packet_num=30
z11_cserver_udp  |
z11_cserver_udp  | Shutdown requested. Total received packets with confirmation response: 86
z11_cserver_udp exited with code 0
```

### Logi klienta

```
z11_cclient_udp  | Applying netem: delay=1000ms jitter=500ms loss=50%
z11_cclient_udp  | Resolving hostname via DNS: cserver
z11_cclient_udp  | Running in RETRANSMIT_ENABLED mode
z11_cclient_udp  |
z11_cclient_udp  |
z11_cclient_udp  | Sending packet 1 with seq_bit=0
z11_cclient_udp  | Sent datagram with seq_bit: 0
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '0'
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '0'
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '0'
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '0'
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '0'
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '0'
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '0'
z11_cclient_udp  | Received correct ACK(0)
z11_cclient_udp  | Received reply (seq_bit 0, packet_num: 1): "ACK"
z11_cclient_udp  |
z11_cclient_udp  | Sending packet 2 with seq_bit=1
z11_cclient_udp  | Sent datagram with seq_bit: 1
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '1'
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '1'
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '1'
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '1'
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '1'
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '1'
z11_cclient_udp  | Received correct ACK(1)
z11_cclient_udp  | Received reply (seq_bit 1, packet_num: 2): "ACK"
z11_cclient_udp  |
z11_cclient_udp  | Sending packet 3 with seq_bit=0
z11_cclient_udp  | Sent datagram with seq_bit: 0
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '0'
z11_cclient_udp  | Wrong ACK (expected 0, got 1), resending
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '0'
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '0'
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '0'
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '0'
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '0'
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '0'
z11_cclient_udp  | Received correct ACK(0)
z11_cclient_udp  | Received reply (seq_bit 0, packet_num: 3): "ACK"
...
z11_cclient_udp  | Sending packet 28 with seq_bit=1
z11_cclient_udp  | Sent datagram with seq_bit: 1
z11_cclient_udp  | Wrong ACK (expected 1, got 0), resending
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '1'
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '1'
z11_cclient_udp  | Received correct ACK(1)
z11_cclient_udp  | Received reply (seq_bit 1, packet_num: 28): "ACK"
z11_cclient_udp  |
z11_cclient_udp  | Sending packet 29 with seq_bit=0
z11_cclient_udp  | Sent datagram with seq_bit: 0
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '0'
z11_cclient_udp  | Wrong ACK (expected 0, got 1), resending
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '0'
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '0'
z11_cclient_udp  | Received correct ACK(0)
z11_cclient_udp  | Received reply (seq_bit 0, packet_num: 29): "ACK"
z11_cclient_udp  |
z11_cclient_udp  | Sending packet 30 with seq_bit=1
z11_cclient_udp  | Sent datagram with seq_bit: 1
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '1'
z11_cclient_udp  | Wrong ACK (expected 1, got 0), resending
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '1'
z11_cclient_udp  | Received correct ACK(1)
z11_cclient_udp  | Received reply (seq_bit 1, packet_num: 30): "ACK"
z11_cclient_udp  |
z11_cclient_udp  | Total sent packets: 157
z11_cclient_udp  | Successfully delivered packets: 30
z11_cclient_udp  | Failed packets: 0
z11_cclient_udp  | Retransmissions: 127
z11_cclient_udp exited with code 0
```
