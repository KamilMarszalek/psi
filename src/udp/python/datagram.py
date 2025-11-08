from dataclasses import dataclass
import struct


@dataclass
class Datagram:
    length: int
    content: bytes

    @property
    def fmt(self) -> str:
        return f"<H{self.length}s"

    def to_bytes(self) -> bytes:
        padded = self.content.ljust(self.length, b"\x00")
        return struct.pack(self.fmt, self.length, padded)

    @classmethod
    def from_bytes(cls, b: bytes):
        length = struct.unpack("<H", b[:2])[0]
        fmt = f"<H{length}s"
        length, content = struct.unpack(fmt, b)
        return cls(length, content.rstrip(b"\x00"))
