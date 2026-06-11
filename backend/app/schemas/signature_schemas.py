from pydantic import BaseModel


class RsaSignatureResponse(BaseModel):
    signature: str


class RsaSignature(BaseModel):
    file_content: str
    login: str
    password: str
    encrypted_private_key: str
    key_module: str
    salt_length: str
    hash_function_1: str
    hash_function_2: str


class VerifyRsaSignatureResponse(BaseModel):
    is_valid: bool


class VerifyRsaSignature(BaseModel):
    file_content: str
    signature: str
    login: str
    salt_length: str
    hash_function_1: str
    hash_function_2: str


class ElgamalSignatureResponse(BaseModel):
    signature: str


class ElgamalSignature(BaseModel):
    file_content: str
    login: str
    password: str
    encrypted_private_key: str
    key_module: str
    hash: str


class VerifyElgamalSignatureResponse(BaseModel):
    is_valid: bool


class VerifyElgamalSignature(BaseModel):
    file_content: str
    signature: str
    login: str
    hash: str
