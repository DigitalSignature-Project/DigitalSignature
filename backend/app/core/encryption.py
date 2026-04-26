import base64
import os
import json

from cryptography.hazmat.primitives.kdf.scrypt import Scrypt
from cryptography.hazmat.primitives.ciphers.aead import AESGCM
from cryptography.exceptions import InvalidTag


def derive_key(password: str, salt: bytes) -> bytes:
    kdf = Scrypt(
        salt=salt,
        length=32,
        n=2**14,
        r=8,
        p=1,
    )
    return kdf.derive(password.encode())


def encrypt(data: str, password: str) -> str:
    salt = os.urandom(16)
    key = derive_key(password, salt)

    aesgcm = AESGCM(key)
    nonce = os.urandom(12)

    ciphertext = aesgcm.encrypt(nonce, data.encode(), None)

    payload = {
        "s": base64.b64encode(salt).decode(),
        "n": base64.b64encode(nonce).decode(),
        "c": base64.b64encode(ciphertext).decode(),
    }

    json_str = json.dumps(payload)

    return base64.b64encode(json_str.encode()).decode()


def decrypt(token: str, password: str) -> str:
    json_str = base64.b64decode(token).decode()
    payload = json.loads(json_str)

    salt = base64.b64decode(payload["s"])
    nonce = base64.b64decode(payload["n"])
    ciphertext = base64.b64decode(payload["c"])

    key = derive_key(password, salt)

    aesgcm = AESGCM(key)
    decrypted = aesgcm.decrypt(nonce, ciphertext, None)

    return decrypted.decode()


def decrypt_bool(token: str, password: str) -> bool:
    try:
        json_str = base64.b64decode(token).decode()
        payload = json.loads(json_str)

        salt = base64.b64decode(payload["s"])
        nonce = base64.b64decode(payload["n"])
        ciphertext = base64.b64decode(payload["c"])

        key = derive_key(password, salt)

        aesgcm = AESGCM(key)
        decrypted = aesgcm.decrypt(nonce, ciphertext, None)

        return True
    except (InvalidTag, ValueError, KeyError):
        return False
