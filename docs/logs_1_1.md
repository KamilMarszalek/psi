<!--
---
title: "PSI - Zadanie 1.1 Logi"
author:
  - "**Damian D'Souza** (lider zespołu)"
  - "Kamil Marszałek"
  - "Michał Szwejk"
date: "2025-11-18"
subtitle: "Zespół Z11"
version: "1.0"
---
-->

# Zadanie 1.1

Do uruchomiena został użyty skrypt `run-with-logs.sh`, który zapisał logi z kontenerów do plików, z powodu rozmiarów logów w pliku zostały umieszczone skrócone wersje

### Logi - Python Server & C Client

```
z11_pserver_to_c_udp  | Python UDP Server Starting...
z11_pserver_to_c_udp  | Server listening on 0.0.0.0:8000
z11_pserver_to_c_udp  | Received datagram (65002 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCD..."
z11_pserver_to_c_udp  | Received datagram (65003 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDE..."
...
z11_pserver_to_c_udp  | Received datagram (65506 bytes): "ABCDEFGHIJKLMNOPQRSTUVW..."
z11_pserver_to_c_udp  | Received datagram (65507 bytes): "ABCDEFGHIJKLMNOPQRSTUVWX..."
```

### Logi - Python Server & Python Client

```
z11_pserver_to_p_udp  | Python UDP Server Starting...
z11_pserver_to_p_udp  | Server listening on 0.0.0.0:8000
z11_pserver_to_p_udp  | Received datagram (65002 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCD..."
z11_pserver_to_p_udp  | Received datagram (65003 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDE..."
...
z11_pserver_to_p_udp  | Received datagram (65506 bytes): "ABCDEFGHIJKLMNOPQRSTUVW..."
z11_pserver_to_p_udp  | Received datagram (65507 bytes): "ABCDEFGHIJKLMNOPQRSTUVWX..."
```

### Logi - C Server & Python Client

```
z11_cserver_to_p_udp  | C UDP Server Starting...
z11_cserver_to_p_udp  | UDP server listening on port 8080...
z11_cserver_to_p_udp  | Received datagram (65002 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCD..."
z11_cserver_to_p_udp  | Received datagram (65003 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDE..."
...
z11_cserver_to_p_udp  | Received datagram (65506 bytes): "ABCDEFGHIJKLMNOPQRSTUVW..."
z11_cserver_to_p_udp  | Received datagram (65507 bytes): "ABCDEFGHIJKLMNOPQRSTUVWX..."
```

### Logi - C Server & C Client

```
z11_cserver_to_c_udp  | C UDP Server Starting...
z11_cserver_to_c_udp  | UDP server listening on port 8080...
z11_cserver_to_c_udp  | Received datagram (65002 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCD..."
z11_cserver_to_c_udp  | Received datagram (65003 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDE..."
...
z11_cserver_to_c_udp  | Received datagram (65506 bytes): "ABCDEFGHIJKLMNOPQRSTUVW..."
z11_cserver_to_c_udp  | Received datagram (65507 bytes): "ABCDEFGHIJKLMNOPQRSTUVWX..."
```

### Logi - C Client & Python Server

```
z11_cclient_to_p_udp  | C UDP Client Starting...
z11_cclient_to_p_udp  | Resolving hostname via DNS: pserver_to_c
z11_cclient_to_p_udp  | Received datagram (65002 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGH..."
z11_cclient_to_p_udp  | Received datagram (65003 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHI..."
...
z11_cclient_to_p_udp  | Received datagram (65506 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMN..."
z11_cclient_to_p_udp  | Received datagram (65507 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNO..."
z11_cclient_to_p_udp  | failed to send datagram (length=65508)
```

### Logi - C Client & C Server

```
z11_cclient_to_c_udp  | C UDP Client Starting...
z11_cclient_to_c_udp  | Resolving hostname via DNS: cserver_to_c
z11_cclient_to_c_udp  | Received datagram (65002 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGH..."
z11_cclient_to_c_udp  | Received datagram (65003 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHI..."
...
z11_cclient_to_c_udp  | Received datagram (65506 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMN..."
z11_cclient_to_c_udp  | Received datagram (65507 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNO..."
z11_cclient_to_c_udp  | failed to send datagram (length=65508)
```

### Logi - Python Client & Python Server

```
z11_pclient_to_p_udp  | Python UDP Client Starting...
z11_pclient_to_p_udp  | Will send to pserver_to_p : 8000
z11_pclient_to_p_udp  | Received datagram (65002 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGH..."
z11_pclient_to_p_udp  | Received datagram (65003 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHI..."
...
z11_pclient_to_p_udp  | Received datagram (65506 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMN..."
z11_pclient_to_p_udp  | Received datagram (65507 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNO..."
z11_pclient_to_p_udp  | Datagram too big to be sent, length 65508 -> [Errno 90] Message too long"

```

### Logi - Python Client & C Server

```
z11_pclient_to_c_udp  | Python UDP Client Starting...
z11_pclient_to_c_udp  | Will send to cserver_to_p : 8080
z11_pclient_to_c_udp  | Received datagram (65002 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGH..."
z11_pclient_to_c_udp  | Received datagram (65003 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHI..."
...
z11_pclient_to_c_udp  | Received datagram (65506 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMN..."
z11_pclient_to_c_udp  | Received datagram (65507 bytes): "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNO..."
z11_pclient_to_c_udp  | Datagram too big to be sent, length 65508 -> [Errno 90] Message too long"
```
