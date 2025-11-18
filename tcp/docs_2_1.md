# PSI Laboratorium - zadanie 2.1a - Komunikacja TCP

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
``` c
typedef struct Node {
  struct Node* left;
  struct Node* right;
  char* text;
  uint32_t text_length;
  int32_t number32;
  int16_t number16;
} node_t;

```
Chcąc przesłać strukturę z wykorzystaniem protokółów sieciowych trzeba najpierw zmienić jej reprezentację na ciąg bajtów. W tym celu wszystkie węzły drzewa są odwiedzane w kolejności pre-order i wpisywane do bufora danych. Takie podejście pozwala ograniczyć liczbę przesyłanych danych, ponieważ w buforze nie trzeba trzymać danych organizacyjnych.

Serializacja każdego węzła przebiega w następujący sposób:
``` c
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
Na początku do bufora wpisywany jest nagłówek (*null_flag*), który wskazuje czy węzeł jest potomkiem liścia. Jeśli nim nie jest trzeba dodatkowo zapisać jego dane (liczby, tekst etc.). Funkcja buffer_write_bytes odpowiada za zapisanie danych do bufora i automatyczne przesunięcie wskaźnika, dzięki czemu kolejne dane trafiają zawsze pod właściwy adres.

Deserializacja węzła przebiega analogicznie - najpierw odczytywana jest flaga opsiująca czy węzeł jest on rzeczywisty, a następnie (o ile istnieją) jego dane. Całe drzewo powstaje po przez odczytywanie kolejnych wezłów i dodawanie ich do drzewa w kolejności pre-order.

#### Komunikacja TCP
Komunikacja TCP pomiędzy klientem a serwerem odbywa się w dwóch etapach:
1. Klient przelicza ile bajtów zajmie całe zbuforowane drzewo, a następnie przesyła tę informację (jedna liczba całkowita, o rozmiarze 32 bitów) serwerowi, dzięki czemu może on zaalokować odpowiednią ilość pamięci na całą strukturę.
2. Klient przesyła kolejne bajty w pętli tak długo, aż nie prześle tylu, ile zajmuje całe drzewo. W tym celu wykorzystywane są wartości zwracane z funkcji *send*, mówiące o tym, jak dużo bajtów zostało przesłane.

Serwer działa tak długo, jak nie otrzyma sygnału kończącego (w nieskończonej pętli odbiera kolejne drzewa pochodzące z różnych źródeł). Klient natomiast po wysłaniu drzewa zamyka gniazdo, zwalania pamięc i kończy działanie.

## Opis konfiguracji sieciowej
Do konfiguracji sieciowej wykorzystano środowisko Docker Comopse - zdefiniowano odpowiednie kontenery (wykorzystujące standardowe obrazy *gcc*), zarówno dla klienta jak i serwera. Oba działają w sieci dockerowej **z11_network** na serwerze **bigubu**. W przypadku konfiguracji serwera dla sieci zdefniowano alias, dzięki takiemu roziwązaniu klient może łatwiej odnaleźć serwer wykorzystując domenę (resolver DNS). Serwer działa na porcie 8080.

Skrócona zawartość pliku *docker-compose.yaml*:
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
      z11_network:
    command: ["server_network", "8080"]
    depends_on: [tcp_server]

networks:
  z11_network:
```
## Testowanie
W celu przetestowania rozwiązania klient tworzy duże, losowe i zrównoważone drzewo składające się z około 3000 węzłów. Taka struktura jest stosunkowo duża, ma więcej niż 100KB. Jest ono przesyłane po TCP zgodnie z opisem znajdującym się wyżej. Następnie, zarówno po stronie klienta jak i serwera, na standardowe wyjście są drukowane 3 pierwsze poziomy drzewa (cała struktura byłaby zbyt duża) w kolejności level-order - pozwala to ocenić poprawność komunikacji.

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
z11_tcp_server  | N0: {int_num: 585, short_num: 290, text_length: 53, text: 4DyLXPPo2AY5O6gVCXSHHnOKSgZx37RXydylRbxHAmBegHZItGeY}
z11_tcp_server  | N1: {int_num: 735, short_num: 23, text_length: 51, text: AWnVNjCyHDWTiWeQmqhqvMd13J5RrLgRvrC8A2w8vhPE3Tid8n}
z11_tcp_server  | N2: {int_num: 940, short_num: 232, text_length: 6, text: E1Ty7}
z11_tcp_server  | N3: {int_num: 610, short_num: 284, text_length: 30, text: uiR2cyjNE09Wq7kisiBzV5qDbtzoK}
z11_tcp_server  | N4: {int_num: 446, short_num: 301, text_length: 11, text: Wk6PHG6WtQ}
z11_tcp_server  | N5: {int_num: 544, short_num: 60, text_length: 64, text: QS4IRYQu4QqznDjFxpGtWxipo2NlA1ChKuNpjrJb7ZyktHQOUm7FHfUV5x4wouB}
z11_tcp_server  | N6: {int_num: 474, short_num: 419, text_length: 59, text: c1BoyGRMYoQS9zCY1fiWd2JdyzpAQtESi65GAMioYY4XnwLcCRWDJvH5TU}
```
Logi klienta:
```
z11_tcp_client  | TCP client socket created.
z11_tcp_client  | Resolving address via DNS: server_network
z11_tcp_client  | Connected to server socket.
z11_tcp_client  |
z11_tcp_client  | Created random binary tree composed of 3000 nodes.
z11_tcp_client  | First 3 levels (level order):
z11_tcp_client  | N0: {int_num: 585, short_num: 290, text_length: 53, text: 4DyLXPPo2AY5O6gVCXSHHnOKSgZx37RXydylRbxHAmBegHZItGeY}
z11_tcp_client  | N1: {int_num: 735, short_num: 23, text_length: 51, text: AWnVNjCyHDWTiWeQmqhqvMd13J5RrLgRvrC8A2w8vhPE3Tid8n}
z11_tcp_client  | N2: {int_num: 940, short_num: 232, text_length: 6, text: E1Ty7}
z11_tcp_client  | N3: {int_num: 610, short_num: 284, text_length: 30, text: uiR2cyjNE09Wq7kisiBzV5qDbtzoK}
z11_tcp_client  | N4: {int_num: 446, short_num: 301, text_length: 11, text: Wk6PHG6WtQ}
z11_tcp_client  | N5: {int_num: 544, short_num: 60, text_length: 64, text: QS4IRYQu4QqznDjFxpGtWxipo2NlA1ChKuNpjrJb7ZyktHQOUm7FHfUV5x4wouB}
z11_tcp_client  | N6: {int_num: 474, short_num: 419, text_length: 59, text: c1BoyGRMYoQS9zCY1fiWd2JdyzpAQtESi65GAMioYY4XnwLcCRWDJvH5TU}
z11_tcp_client  |
z11_tcp_client  | Tree size (134383 bytes) sent.
z11_tcp_client  | Serialized tree sent. 
z11_tcp_client  | Closing connection.
```