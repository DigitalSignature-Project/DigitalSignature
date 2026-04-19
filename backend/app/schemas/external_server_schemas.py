from pydantic import BaseModel


class RegisterNewUserResponse(BaseModel):
    status_code: int


class RegisterNewUser(BaseModel):
    login: str
    password_hash: str
    public_key: str
    encrypted_private_key: str


class VerifyUserLoginResponse(BaseModel):
    message: str
    encrypted_private_key: str
    public_key: str


class VerifyUserLogin(BaseModel):
    login: set
    password_hash: str
