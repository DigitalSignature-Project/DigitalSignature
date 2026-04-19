from pydantic import BaseModel


class RegisterNewUserResponse(BaseModel):
    status_code: int
    
    
class RegisterNewUser(BaseModel):
    login: str
    password_hash: str
    public_key: str
    encrypted_private_key: str