<!--
---
title: "PSI - Programowanie Systemów i Sieci - Laboratorium"
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

## Treść zadania
Napisz zestaw dwóch programów – klienta i serwera wysyłające datagramy UDP. Wykonaj ćwiczenie w kolejnych
inkrementalnych wariantach (rozszerzając kod z poprzedniej wersji). Klient oraz serwer powinien być napisany
zarówno w C jak i Pythonie (4 programy).
Sprawdzić i przetestować działanie „między-platformowe”, tj. klient w C z serwerem Python i vice versa. <br>
Klient wysyła, a serwer odbiera datagramy o stałym rozmiarze (rzędu kilkuset bajtów). Datagramy powinny posiadać
ustaloną formę danych. Przykładowo: pierwsze dwa bajty datagramu mogą zawierać informację o jego długości, a
kolejne bajty kolejne litery A-Z powtarzające się wymaganą liczbę razy (ale można przyjąć inne rozwiązanie). Odbiorca
powinien weryfikować odebrany datagram i odsyłać odpowiedź o ustalonym formacie. Klient powinien wysyłać
kolejne datagramy o przyrastającej wielkości np. 1, 100, 200, 1000, 2000… bajtów. Sprawdzić, jaki był maksymalny
rozmiar wysłanego (przyjętego) datagramu. Ustalić eskperymentalnie z dokładnością do jednego bajta jak duży
datagram jest obsługiwany. Wyjaśnić.

## Opis rozwiązania
W ramach realizacji zadania zostały stworzone cztery programy: klient i serwer UDP w języku C oraz klient i serwer UDP w języku Python. Każdy z programów został zaprojektowany tak, aby umożliwić wysyłanie i odbieranie datagramów UDP o zmiennej wielkości. Datagramy są konstruowane w taki sposób, że pierwsze dwa bajty zawierają informację o długości datagramu, a pozostałe bajty są wypełnione powtarzającymi się literami od A do Z. W języku Python tworzenie datagramu odbywa się za pomocą użycia modułów dataclasses oraz struct. Zostały zaimplementowane funkcje to_bytes oraz from_bytes do konwersji obiektów datagramów na bajty i odwrotnie.

```python
@dataclass
class Datagram:
    length: int
    content: bytes

    def to_bytes(self) -> bytes:
        return struct.pack(self.fmt, self.length, self.content)

    @classmethod
    def from_bytes(cls, b: bytes):
        length = struct.unpack("!H", b[:2])[0]
        fmt = f"!H{length}s"
        size = struct.calcsize(fmt)
        length, content = struct.unpack(fmt, b[:size])
        return cls(length, content.rstrip(b"\x00"))
```

W języku C struktura datagramu została zdefiniowana jako struct Datagram. Analogicznie jak w pythonie zostały zaimplementowane funkcje do konwersji struktury na bajty i odwrotnie.

```c
typedef struct {
  unsigned short length;
  char *content;
} Datagram;

Datagram *from_bytes(const char *bytes, size_t size);
char *to_bytes(const Datagram *d, size_t *out_size);
```

Rozwiązanie problemu w języku C wymagało użycia resolvera dns


Programy klienckie zostały zaprojektowane tak, aby wysyłać datagramy o rosnącej wielkości, zaczynając od 65000 bajtów i zwiększając rozmiar o 1 bajt w każdej iteracji, aż do momentu, gdy wysłanie datagramu zakończy się niepowodzeniem. Serwery odbierają datagramy, weryfikują ich poprawność i wypisują informacje o odebranych danych na standardowe wyjście. Następnie serwer odsyła ten sam datagram z powrotem do klienta jako potwierdzenie odbioru. Klient odbiera odpowiedź i weryfikuje jej zgodność z wysłanym datagramem. Jeśli otrzymany datagram jest taki sam jak wysłany, klient kontynuuje wysyłanie kolejnego większego datagramu. W przypadku niezgodności lub błędu wysyłki, klient przerywa działanie i raportuje maksymalny rozmiar poprawnie obsłużonego datagramu.

## Wyniki eksperymentu
Maksymalny rozmiar datagramu UDP, który został poprawnie obsłużony w naszych testach to 65507 bajtów. Jest to zgodne z teoretycznym limitem, ponieważ maksymalny rozmiar pakietu IP to 65535 bajtów, a nagłówki IP i UDP zajmują odpowiednio 20 i 8 bajtów, pozostawiając 65507 bajtów na dane użytkownika.

## Sprawdzenie międzyplatformowe
Testy międzyplatformowe zostały przeprowadzone pomyślnie. Klient napisany w C poprawnie komunikował się z serwerem napisanym w Pythonie i odwrotnie. W obu przypadkach datagramy były poprawnie wysyłane i odbierane, a ich zawartość była zgodna z oczekiwaniami.

## Opis konfiguracji testowej
Do przeprowadzenia testów użyliśmy środowiska Docker Compose, które umożliwiło łatwe zarządzanie kontenerami dla każdego z programów. Poniżej znajduje się przykładowa konfiguracja pliku docker-compose.yml użyta do uruchomienia testów:

```yaml
services:

  cserver_to_c:
    build:
      context: ./c
      dockerfile: dockerfile.server
    image: z11_cserver_udp
    container_name: z11_cserver_to_c_udp
    hostname: cserver_to_c
    networks:
      z11_network: #sieć z11 odpowiadająca numerowi zespołu
        aliases: [cserver_to_c]
    command: ["8080"]
# W sposób analogiczny skonfigurowano pozostałe usługi: cserver_to_p, cclient_to_c, cclient_to_p, pserver_to_c, pserver_to_p, pclient_to_c, pclient_to_p
```

## Opis testowania i wydruki z testów
Dla każdej z kombinacji klient-serwer (Python-Python, Python-C, C-Python, C-C) zostały przeprowadzone testy wysyłania i odbierania datagramów UDP o rosnącej wielkości. Poniżej znajdują się fragmenty logów z poszczególnych testów, które ilustrują przebieg eksperymentu.

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

# Zadanie 1.2

## Treść zadania
Wychodzimy z kodu z zadania 1.1, tym razem pakiety datagramu mają stałą wielkość, można przyjąć np. 512B. Należy zaimplementować prosty protokół niezawodnej transmisji, uwzględniający możliwość gubienia datagramów. Rozszerzyć protokół i program tak, aby gubione pakiety były wykrywane i retransmitowane. Wskazówka – "Bit alternate protocol". Należy uruchomić program w środowisku symulującym błędy gubienia pakietów. (Informacja o tym, jak to zrobić znajduje się w skrypcie opisującym środowisko Dockera).

To zadanie można wykonać, korzystając z kodu klienta i serwera napisanych w C lub w Pythonie (do wyboru). Nie trzeba tworzyć wersji w obydwu językach.

## Opis rozwiązania

### Struktura datagramu
W ramach realizacji zadania zaprojektowano strukturę datagramu pozwalającą na niezawodną transmisję danych przez protokół UDP:

```c
typedef struct {
  struct header {
    unsigned char seq_bit : 1;
    unsigned short packet_num;
  } header;
  unsigned short length;
  char *content;
} Datagram;
```

### Implementacja Bit Alternating Protocol

#### Mechanizm działania protokołu

Bit Alternating Protocol zapewnia niezawodną transmisję przez UDP wykorzystując pojedynczy bit sekwencyjny (0 lub 1) oraz mechanizm potwierdzeń. Klient wysyła datagram z określonym bitem sekwencyjnym i czeka na potwierdzenie zawierające ten sam bit. Jeśli potwierdzenie nie nadejdzie w określonym czasie lub nadejdzie z niewłaściwym bitem, klient retransmituje pakiet. Po otrzymaniu prawidłowego ACK, bit sekwencyjny jest odwracany i następuje transmisja kolejnego pakietu. Serwer odbiera datagramy i odpowiada potwierdzeniem zawierającym ten sam bit sekwencyjny, co pozwala klientowi wykryć spóźnione ACK z poprzednich transmisji.

**Klient (nadawca)**:
```c
void send_and_receive(int sockfd, struct sockaddr_in *server, int msg_len,
                      unsigned char seq_bit, unsigned short packet_num) {
  Datagram *d = create_datagram(seq_bit, packet_num, msg_len, message);
  char *packet = to_bytes(d, &packet_size);
  sendto(sockfd, packet, packet_size, 0, ...);

  while (1) {
    n = recvfrom(sockfd, recv_buffer, BUFFER_SIZE, 0, NULL, NULL);

    if (n < 0 && errno == EAGAIN) {
      printf("Timeout, resending datagram with seq_bit: '%u'\n", seq_bit);
      sendto(sockfd, packet, packet_size, 0, ...);
      continue;
    }

    resp = from_bytes(recv_buffer, n);
    if (resp->header.seq_bit == seq_bit) {
      printf("Received correct ACK(%u)\n", resp->header.seq_bit);
      break;
    } else {
      printf("Wrong ACK (expected %u, got %u), resending\n",
             seq_bit, resp->header.seq_bit);
      sendto(sockfd, packet, packet_size, 0, ...);
    }
  }
}
```

**Serwer (odbiorca)**:
```c
void handle_echo(int sockfd) {
  ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                       (struct sockaddr *)&client, &client_len);

  Datagram *d = from_bytes(buffer, n);
  printf("Received datagram (seq_bit=%u, packet_num=%d)\n",
         d->header.seq_bit, d->header.packet_num);

  Datagram *ack = create_datagram(d->header.seq_bit, d->header.packet_num,
                                  sizeof("ACK") - 1, "ACK");
  char *reply = to_bytes(ack, &out_size);
  sendto(sockfd, reply, out_size, 0, (struct sockaddr *)&client, client_len);
}
```

Serwer odpowiada potwierdzeniem ACK z tym samym bitem sekwencyjnym, który otrzymał w datagramie. Serwer nie przechowuje stanu - zawsze odpowiada na każdy odebrany datagram, niezależnie od tego, czy jest duplikatem.

## Napotkane problemy i rozwiązania

### Problem 1: Identyfikacja spóźnionych potwierdzeń

**Problem**: W środowisku z dużym opóźnieniem i utratą pakietów ACK z poprzednich transmisji mogą docierać po wysłaniu kolejnego pakietu z innym seq_bit.

**Rozwiązanie**: Implementacja weryfikacji seq_bit po stronie klienta. Klient porównuje otrzymany seq_bit z oczekiwanym i odrzuca niepasujące potwierdzenia, wykonując retransmisję:

```c
if (resp->header.seq_bit == seq_bit) {
  ...
} else {
  printf("Wrong ACK (expected %u, got %u)", seq_bit, resp->header.seq_bit);
  if (retransmit_mode == RETRANSMIT_ENABLED) {
    sendto(sockfd, packet, packet_size, 0, ...);
  }
}
```

## Konfiguracja testowa

### Środowisko Docker

Do przeprowadzenia testów użyto środowiska Docker Compose z kontenerami dla serwera i klienta z możliwością ustawienia parametrów dla netem za pomoca zmiennych środowiskowych:

```yaml
services:
  cserver:
    build:
      context: .
      dockerfile: dockerfile.server
    image: z11_cserver_udp
    container_name: z11_cserver_udp
    cap_add:
      - NET_ADMIN
    hostname: cserver
    networks:
      z11_network:
        aliases:
          - cserver
    environment:
      - NETEM_ENABLED=${NETEM_ENABLED:-false}
      - NETEM_DELAY=${NETEM_DELAY:-0ms}
      - NETEM_JITTER=${NETEM_JITTER:-0ms}
      - NETEM_LOSS=${NETEM_LOSS:-0%}
    command: ["8080"]
```

### Symulacja błędów sieci - NetEm

Zarówno serwer jak i klient uruchamiają NetEm (Network Emulator) na interfejsie sieciowym `eth0` przy starcie kontenera.

Parametry NetEm użyte w testach:
```bash
tc qdisc add dev eth0 root netem delay 1000ms 500ms loss 50%
```

## Opis testowania i wydruki z testów

### Przebieg testów

Przeprowadzono testy przesłania 30 pakietów po 512 bajtów każdy w środowisku z symulacją błędów sieciowych. Testy wykonano w dwóch trybach:

1. **RETRANSMIT_DISABLED** - standardowy UDP
2. **RETRANSMIT_ENABLED** - z protokołem Bit Alternating Protocol

### Test 1: Tryb bez retransmisji (RETRANSMIT_DISABLED)

W tym trybie klient wysyła pakiet tylko raz i czeka na ACK. Jeśli ACK nie przyjdzie w ciągu timeoutu zostaje uznany za zgubiony.

#### Logi - Klient (bez retransmisji)
##### Odpowiedź nie dotarła
```
z11_cclient_udp  | Sending packet 1 with seq_bit=0
z11_cclient_udp  | Sent datagram with seq_bit: 0
z11_cclient_udp  | Timeout, no retransmission (packet lost)
z11_cclient_udp  |
z11_cclient_udp  | Sending packet 2 with seq_bit=1
z11_cclient_udp  | Sent datagram with seq_bit: 1
z11_cclient_udp  | Timeout, no retransmission (packet lost)
```

##### Opóźniona odpowiedź na inny pakiet i odpowiedź na czas
```
z11_cclient_udp  | Sending packet 4 with seq_bit=1
z11_cclient_udp  | Sent datagram with seq_bit: 1
z11_cclient_udp  | Wrong ACK (expected 1, got 0), packet_num: 1 - no retransmission
z11_cclient_udp  |
z11_cclient_udp  | Sending packet 5 with seq_bit=0
z11_cclient_udp  | Sent datagram with seq_bit: 0
z11_cclient_udp  | Received correct ACK(0)
z11_cclient_udp  | Received reply (seq_bit 0, packet_num: 3): "ACK"
```

#### Logi - Serwer (bez retransmisji)
##### Pakiety dotarły w złej kolejności
```
z11_cserver_udp  | Received datagram (seq_bit=1, packet_num=2): "ABCDEFGH..."
z11_cserver_udp  | Sent ACK with seq_bit=1 for packet_num=2
z11_cserver_udp  |
z11_cserver_udp  | Received datagram (seq_bit=0, packet_num=1): "ABCDEFGH..."
z11_cserver_udp  | Sent ACK with seq_bit=0 for packet_num=1
```

##### Pakiety nie dorarły
```
z11_cserver_udp  | Received datagram (seq_bit=1, packet_num=6): "ABCDEFGH..."
z11_cserver_udp  | Sent ACK with seq_bit=1 for packet_num=6
z11_cserver_udp  |
z11_cserver_udp  | Received datagram (seq_bit=1, packet_num=10): "ABCDEFGH..."
z11_cserver_udp  | Sent ACK with seq_bit=1 for packet_num=10
```

#### Wyniki testu bez retransmisji
```
z11_cclient_udp  | Total sent packets: 30
z11_cclient_udp  | Successfully delivered packets: 7
z11_cclient_udp  | Failed packets: 23

z11_cserver_udp  | Total received packets with confirmation response: 17
```

**Analiza**:

- 17 z 30 pakietów (57%) dotarło do serwera, jednak potwierdzenie ACK z powrotem do klienta dotarło tylko dla 7 z nich (23%). Oznacza to, że:
- 13 pakietów (43%) zgubiło się w drodze do serwera
- 10 pakietów (33%) dotarło do serwera, ale ich ACK zginął w drodze powrotnej
- 7 pakietów (23%) zostało pomyślnie dostarczonych z potwierdzeniem

Bez mechanizmu retransmisji tylko 23% komunikacji zakończyło się sukcesem, mimo że ponad połowa pakietów faktycznie dotarła do celu.

### Test 2: Tryb z retransmisją (RETRANSMIT_ENABLED - Bit Alternating Protocol)

W tym trybie klient retransmituje pakiety, które nie otrzymały ACK w określonym czasie lub otrzymały nieprawidłowy ACK.

#### Logi - Klient (z retransmisją)
##### Wielokrotna retransmisja z powodu timeoutu
```
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
```

##### Odrzucenie spóźnionego ACK
```
z11_cclient_udp  | Sending packet 3 with seq_bit=0
z11_cclient_udp  | Sent datagram with seq_bit: 0
z11_cclient_udp  | Timeout, resending datagram with seq_bit: '0'
z11_cclient_udp  | Wrong ACK (expected 0, got 1), resending
...
z11_cclient_udp  | Received correct ACK(0)
z11_cclient_udp  | Received reply (seq_bit 0, packet_num: 3): "ACK"
```

##### Przykład 3: Natychmiastowy sukces
```
z11_cclient_udp  | Sending packet 10 with seq_bit=1
z11_cclient_udp  | Sent datagram with seq_bit: 1
z11_cclient_udp  | Received correct ACK(1)
z11_cclient_udp  | Received reply (seq_bit 1, packet_num: 8): "ACK"
```

#### Logi - Serwer (z retransmisją)

##### Przykład: Serwer otrzymuje ten sam pakiet wielokrotnie
```
z11_cserver_udp  | Received datagram (seq_bit=0, packet_num=1): "ABCDEFGH..."
z11_cserver_udp  | Sent ACK with seq_bit=0 for packet_num=1
z11_cserver_udp  |
z11_cserver_udp  | Received datagram (seq_bit=0, packet_num=1): "ABCDEFGH..."
z11_cserver_udp  | Sent ACK with seq_bit=0 for packet_num=1
```

#### Wyniki testu z retransmisją
```
z11_cclient_udp  | Total sent packets: 157
z11_cclient_udp  | Successfully delivered packets: 30
z11_cclient_udp  | Failed packets: 0
z11_cclient_udp  | Retransmissions: 127

z11_cserver_udp  | Total received packets with confirmation response: 86
```

**Analiza wyników z retransmisją**:

  - Wszystkie 30 pakietów (100%) zostało pomyślnie dostarczonych z potwierdzeniem
  - Łącznie wysłano 157 pakietów (30 unikalnych + 127 retransmisji)
  - Serwer odebrał 86 pakietów (w tym duplikaty przez retransmisje)

# Zadanie 2.1a

## Treść zadania
Napisz zestaw dwóch programów – klienta i serwera komunikujących się poprzez TCP. Transmitowany strumień
danych powinien być stosunkowo duży, nie mniej niż 100 kB.
Klient TCP napisany w C wysyła złożoną strukturę danych. Przykładowo: tworzymy w pamięci listę jednokierunkową
lub drzewo binarne struktur zawierających (oprócz danych organizacyjnych) pewne dane dodatkowe: np. liczbę
całkowitą 16-o bitową, liczbę całkowitą 32-u bitową oraz napis zmiennej i ograniczonej długości. Serwer napisany w
Pythonie/C powinien te dane odebrać, dokonać poprawnego „odpakowania” tej struktury i wydrukować jej pola (być
może w skróconej postaci, aby uniknąć nadmiaru wyświetlanych danych).

## Opis rozwiązania
#### Struktura danych
 Jako złożoną strukturę danych (przesyłaną później po TCP) zdefiniowano drzewo binarne, którego węzły mają następującą strukturę:
```c
typedef struct Node {
  struct Node* left;
  struct Node* right;
  char* text;
  uint32_t text_length;
  int32_t number32;
  int16_t number16;
} node_t;
```
Chcąc przesłać strukturę z wykorzystaniem protokółów sieciowych trzeba najpierw zmienić jej reprezentację na ciąg bajtów. W tym celu wszystkie węzły drzewa są odwiedzane w kolejności pre-order i wpisywane do bufora danych. Takie podejście pozwala ograniczyć liczbę przesyłanych danych, ponieważ w buforze nie trzeba trzymać danych organizacyjnych.

Serializacja każdego węzła przebiega w następujący sposób:
```c
 void node_serialize(const node_t* node, buffer_t* buf) {
  uint8_t null_flag = 0;
  if (node == NULL) {
    buffer_write_bytes(buf, &null_flag, sizeof(null_flag));
    return;
  }
  null_flag = 1;
  buffer_write_bytes(buf, &null_flag, sizeof(null_flag));

  uint32_t text_length_n = htonl(node->text_length);
  uint32_t number32_n = htonl(node->number32);
  uint16_t number16_n = htons(node->number16);

  buffer_write_bytes(buf, &text_length_n, sizeof(text_length_n));
  buffer_write_bytes(buf, &number32_n, sizeof(number32_n));
  buffer_write_bytes(buf, &number16_n, sizeof(number16_n));
  buffer_write_bytes(buf, node->text, node->text_length);
}
```

Na początku do bufora wpisywany jest nagłówek (null_flag), który wskazuje czy węzeł jest potomkiem liścia. Jeśli nim nie jest trzeba dodatkowo zapisać jego dane (liczby, tekst etc.). Funkcja buffer_write_bytes odpowiada za zapisanie danych do bufora i automatyczne przesunięcie wskaźnika, dzięki czemu kolejne dane trafiają zawsze pod właściwy adres.

Deserializacja węzła przebiega analogicznie - najpierw odczytywana jest flaga opsiująca czy węzeł jest on rzeczywisty, a następnie (o ile istnieją) jego dane. Całe drzewo powstaje po przez odczytywanie kolejnych wezłów i dodawanie ich do drzewa w kolejności pre-order.

#### Komunikacja TCP

Komunikacja TCP pomiędzy klientem a serwerem odbywa się w dwóch etapach:

1. Klient przelicza ile bajtów zajmie całe zbuforowane drzewo, a następnie przesyła tę informację (jedna liczba całkowita, o rozmiarze 32 bitów) serwerowi, dzięki czemu może on zaalokować odpowiednią ilość pamięci na całą strukturę.
2. Klient przesyła kolejne bajty w pętli tak długo, aż nie prześle tylu, ile zajmuje całe drzewo. W tym celu wykorzystywane są wartości zwracane z funkcji send, mówiące o tym, jak dużo bajtów zostało przesłane.

Serwer działa tak długo, jak nie otrzyma sygnału kończącego (w nieskończonej pętli odbiera kolejne drzewa pochodzące z różnych źródeł). Klient natomiast po wysłaniu drzewa zamyka gniazdo, zwalania pamięć i kończy działanie.

## Opis konfiguracji sieciowej

Do konfiguracji sieciowej wykorzystano środowisko Docker Comopse - zdefiniowano odpowiednie kontenery (wykorzystujące standardowe obrazy gcc), zarówno dla klienta jak i serwera. Oba działają w sieci dockerowej *z11_network* na serwerze *bigubu*. W przypadku konfiguracji serwera dla sieci zdefniowano alias, dzięki takiemu roziwązaniu klient może łatwiej odnaleźć serwer wykorzystując domenę (resolver DNS). Serwer działa na porcie 8080.

Skrócona zawartość pliku docker-compose.yaml:
```yaml
services:
  tcp_server:
  ...
    networks:
      z11_network:
        aliases: [server_network]
    command: ["8080"]

  tcp_client:
  ...
    networks:
      - z11_network
    command: ["server_network", "8080"]
    depends_on: [tcp_server]

networks:
  z11_network:
   external: true
```

## Testowanie
W celu przetestowania rozwiązania klient tworzy duże, losowe i zrównoważone drzewo składające się z około 3000 węzłów. Taka struktura jest stosunkowo duża, ma więcej niż 100KB. Jest ono przesyłane po TCP zgodnie z opisem znajdującym się wyżej. Następnie, zarówno po stronie klienta jak i serwera, na standardowe wyjście są drukowane 3 pierwsze poziomy drzewa (cała struktura byłaby zbyt duża) w kolejności level-order - pozwala to ocenić poprawność komunikacji.

Logi serwera:
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
z11_tcp_server  | N0: {int_num: 585, short_num: 290, text_length: 53, text: 4DyLXPP...}
z11_tcp_server  | N1: {int_num: 735, short_num: 23, text_length: 51, text: AWnVNjC...}
z11_tcp_server  | N2: {int_num: 940, short_num: 232, text_length: 6, text: E1Ty7}
z11_tcp_server  | N3: {int_num: 610, short_num: 284, text_length: 30, text: uiR2cyj...}
z11_tcp_server  | N4: {int_num: 446, short_num: 301, text_length: 11, text: Wk6PHG6WtQ}
z11_tcp_server  | N5: {int_num: 544, short_num: 60, text_length: 64, text: QS4IRYQ...}
z11_tcp_server  | N6: {int_num: 474, short_num: 419, text_length: 59, text: c1BoyGR...}
```
Logi klienta:
```
z11_tcp_client  | TCP client socket created.
z11_tcp_client  | Resolving address via DNS: server_network
z11_tcp_client  | Connected to server socket.
z11_tcp_client  |
z11_tcp_client  | Created random binary tree composed of 3000 nodes.
z11_tcp_client  | First 3 levels (level order):
z11_tcp_client  | N0: {int_num: 585, short_num: 290, text_length: 53, text: 4DyLXPP...}
z11_tcp_client  | N1: {int_num: 735, short_num: 23, text_length: 51, text: AWnVNjC...}
z11_tcp_client  | N2: {int_num: 940, short_num: 232, text_length: 6, text: E1Ty7}
z11_tcp_client  | N3: {int_num: 610, short_num: 284, text_length: 30, text: uiR2cyj...}
z11_tcp_client  | N4: {int_num: 446, short_num: 301, text_length: 11, text: Wk6PHG6WtQ}
z11_tcp_client  | N5: {int_num: 544, short_num: 60, text_length: 64, text: QS4IRYQ...}
z11_tcp_client  | N6: {int_num: 474, short_num: 419, text_length: 59, text: c1BoyGR...}
z11_tcp_client  |
z11_tcp_client  | Tree size (134383 bytes) sent.
z11_tcp_client  | Serialized tree sent.
z11_tcp_client  | Closing connection.
```
