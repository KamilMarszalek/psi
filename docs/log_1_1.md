# PSI - Laboratorium - zadanie 1.1 - Komunikacja UDP 
## Authors: Z11
* Damian D'Souza
* Kamil Marszałek
* Michał Szwejk

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

