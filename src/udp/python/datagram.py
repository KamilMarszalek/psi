from dataclasses import dataclass
import struct


@dataclass
class Datagram:
    length: int
    content: bytes

    def __post_init__(self):
        if not isinstance(self.content, (bytes, bytearray)):
            raise TypeError("content must be of type bytes")

        if len(self.content) > self.length:
            raise ValueError("content is longer than declared length")

        if len(self.content) < self.length:
            self.content = self.content.ljust(self.length, b"\x00")

    @property
    def fmt(self) -> str:
        return f"<H{self.length}s"

    def to_bytes(self) -> bytes:
        return struct.pack(self.fmt, self.length, self.content)

    @classmethod
    def from_bytes(cls, b: bytes):
        length = struct.unpack("<H", b[:2])[0]
        fmt = f"<H{length}s"
        length, content = struct.unpack(fmt, b)
        return cls(length, content.rstrip(b"\x00"))
