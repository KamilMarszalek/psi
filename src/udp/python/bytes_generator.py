def get_bytes(length: int) -> bytes:
    return bytes((ord("A") + (i % 26)) for i in range(length))
