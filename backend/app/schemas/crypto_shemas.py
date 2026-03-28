from pydantic import BaseModel


class EncryptResponse(BaseModel):
    result: str


class DecryptResponse(BaseModel):
    result: str


class RsaKeyResponse(BaseModel):
    key_pub: str
    key_priv: str
    key_module: str


class RsaKeyParallelResponse(BaseModel):
    key_pub: str
    key_priv: str
    key_module: str


class Encrypt(BaseModel):
    message: int
    pub_key: int
    key_module: int


class Decrypt(BaseModel):
    encrypted_message: int
    priv_key: int
    key_module: int


class RsaKey(BaseModel):
    bits: int


class RsaKeyParallel(BaseModel):
    bits: int
    threads: int
