from datagram import Datagram
from pytest import raises


def test_create_datagram_0_length():
    datagram = Datagram(0, b"")
    assert datagram.length == 0
    assert datagram.content == b""


def test_create_datagram_negative_length():
    with raises(ValueError):
        Datagram(-1, b"")


def test_create_datagram_content_longer_than_length():
    with raises(ValueError):
        Datagram(2, b"abc")


def test_convert_datagram_to_bytes_and_back():
    original_datagram = Datagram(5, b"hello")
    datagram_bytes = original_datagram.to_bytes()
    reconstructed_datagram = Datagram.from_bytes(datagram_bytes)
    assert original_datagram == reconstructed_datagram
