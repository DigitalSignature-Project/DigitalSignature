import httpx
import asyncio

from fastapi import APIRouter, Depends

from app.auth import verify_token
from app.schemas.external_server_schemas import RegisterNewUserResponse, RegisterNewUser


router = APIRouter(dependencies=[Depends(verify_token)])

@router.get("/register_new_user", response_model=RegisterNewUserResponse)
async def register_new_user(data: RegisterNewUser) -> RegisterNewUserResponse:
    payload = {
        "login": data.login,
        "password_hash": data.password_hash,
        "public_key": data.public_key,
        "encrypted_private_key": data.encrypted_private_key
    }
    
    response = httpx.post(
        "https://digital-signature-auth.digitalsignature-auth.workers.dev/api/register",
        json=payload,
        headers={"Content-Type": "application/json"}
    )
    
    return RegisterNewUserResponse(status_code=response.status_code)


    