from pydantic import BaseModel


class RegisterNewUserResponse(BaseModel):
    status_code: int


class RegisterNewUser(BaseModel):
    login: str
    password_hash: str
    public_key: str
    encrypted_private_key: str
    key_module: str
    private_key_user_password: str


class VerifyUserLoginResponse(BaseModel):
    message: str
    encrypted_private_key: str
    public_key: str
    key_module: str


class VerifyUserLogin(BaseModel):
    login: str
    password_hash: str


class RetrievePublicKeyResponse(BaseModel):
    login: str
    public_key: str
    key_module: str


class KeyUpdateResponse(BaseModel):
    status: int


class KeyUpdate(BaseModel):
    login: str
    password_hash: str
    new_public_key: str
    new_encrypted_private_key: str
    new_key_module: str


class ServerStatusResponse(BaseModel):
    status: int


class CheckUserKeyResponse(BaseModel):
    status: int


class CheckUserKey(BaseModel):
    login: str
    password_hash: str
    key: str
