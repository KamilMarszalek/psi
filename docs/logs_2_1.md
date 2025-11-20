<!--
---
title: "PSI - Zadanie 2.1a Logi"
author:
  - "**Damian D'Souza** (lider zespołu)"
  - "Kamil Marszałek"
  - "Michał Szwejk"
date: "2025-11-18"
subtitle: "Zespół Z11"
version: "1.0"
---
-->

# Zadanie 2.1a

Kontenery zostały uruchumione poleceniami

```bash
docker compose up -d tcp_server
docker compose up -d tcp_client
```

Poniższe logi zostały uzyskane z poleceń

```bash
docker compose logs tcp_server
docker compose logs tcp_client
```

### Logi serwera:
```
z11_tcp_server  | TCP server socket created.
z11_tcp_server  | Bind to the port number 8080.
z11_tcp_server  | Listening...
z11_tcp_server  |
z11_tcp_server  | Client connected.
z11_tcp_server  | Received tree size: 134383 bytes.
z11_tcp_server  | Received  serialized tree.
z11_tcp_server  | Successfully deserialized tree.
z11_tcp_server  |
z11_tcp_server  | First 3 levels (level order):
z11_tcp_server  | N0: {int_num: 585, short_num: 290, text_length: 53,
                | text: 4DyLXPPo2AY5O6gVCXSHHnOKSgZx37RXydylRbxHAmBegHZItGeY}
z11_tcp_server  | N1: {int_num: 735, short_num: 23, text_length: 51,
                | text: AWnVNjCyHDWTiWeQmqhqvMd13J5RrLgRvrC8A2w8vhPE3Tid8n}
z11_tcp_server  | N2: {int_num: 940, short_num: 232, text_length: 6,
                | text: E1Ty7}
z11_tcp_server  | N3: {int_num: 610, short_num: 284, text_length: 30,
                | text: uiR2cyjNE09Wq7kisiBzV5qDbtzoK}
z11_tcp_server  | N4: {int_num: 446, short_num: 301, text_length: 11,
                | text: Wk6PHG6WtQ}
z11_tcp_server  | N5: {int_num: 544, short_num: 60, text_length: 64,
                | text: QS4IRYQu4QqznDjFxpGtWxipo2NlA1ChKuNpjrJb7ZyktHQOUm7FHfUV5x4wouB}
z11_tcp_server  | N6: {int_num: 474, short_num: 419, text_length: 59,
                | text: c1BoyGRMYoQS9zCY1fiWd2JdyzpAQtESi65GAMioYY4XnwLcCRWDJvH5TU}
```

### Logi klienta:
```
z11_tcp_client  | TCP client socket created.
z11_tcp_client  | Resolving address via DNS: server_network
z11_tcp_client  | Connected to server socket.
z11_tcp_client  |
z11_tcp_client  | Created random binary tree composed of 3000 nodes.
z11_tcp_client  | First 3 levels (level order):
z11_tcp_client  | N0: {int_num: 585, short_num: 290, text_length: 53,
                | text: 4DyLXPPo2AY5O6gVCXSHHnOKSgZx37RXydylRbxHAmBegHZItGeY}
z11_tcp_client  | N1: {int_num: 735, short_num: 23, text_length: 51,
                | text: AWnVNjCyHDWTiWeQmqhqvMd13J5RrLgRvrC8A2w8vhPE3Tid8n}
z11_tcp_client  | N2: {int_num: 940, short_num: 232, text_length: 6,
                | text: E1Ty7}
z11_tcp_client  | N3: {int_num: 610, short_num: 284, text_length: 30,
                | text: uiR2cyjNE09Wq7kisiBzV5qDbtzoK}
z11_tcp_client  | N4: {int_num: 446, short_num: 301, text_length: 11,
                | text: Wk6PHG6WtQ}
z11_tcp_client  | N5: {int_num: 544, short_num: 60, text_length: 64,
                | text: QS4IRYQu4QqznDjFxpGtWxipo2NlA1ChKuNpjrJb7ZyktHQOUm7FHfUV5x4wouB}
z11_tcp_client  | N6: {int_num: 474, short_num: 419, text_length: 59,
                | text: c1BoyGRMYoQS9zCY1fiWd2JdyzpAQtESi65GAMioYY4XnwLcCRWDJvH5TU}
z11_tcp_client  |
z11_tcp_client  | Tree size (134383 bytes) sent.
z11_tcp_client  | Serialized tree sent.
z11_tcp_client  | Closing connection.
```
