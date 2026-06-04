from pydantic import BaseModel

class RsaSignatureResponse(BaseModel):
    signature: str


class RsaSignature(BaseModel):
    file_content: str
    login: str
    password: str
    encrypted_private_key: str | None = None
    key_module: str | None = None


class VerifyRsaSignatureResponse(BaseModel):
    is_valid: bool


class VerifyRsaSignature(BaseModel):
    file_content: str
    signature: str
    login: str