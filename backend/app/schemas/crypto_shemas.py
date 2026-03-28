from pydantic import BaseModel


class RsaKeyResponse(BaseModel):
    key_pub: str
    key_priv: str
    key_module: str


class RsaKeyParallelResponse(BaseModel):
    key_pub: str
    key_priv: str
    key_module: str


class RsaKey(BaseModel):
    bits: int


class RsaKeyParallel(BaseModel):
    bits: int
    threads: int
