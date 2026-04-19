import httpx
import asyncio

from fastapi import APIRouter, Depends

from app.auth import verify_token
from app.schemas.external_server_schemas import (
    RegisterNewUserResponse,
    RegisterNewUser,
    VerifyUserLogin,
    VerifyUserLoginResponse,
)

router = APIRouter(dependencies=[Depends(verify_token)])


@router.post("/register_new_user", response_model=RegisterNewUserResponse)
async def register_new_user(data: RegisterNewUser) -> RegisterNewUserResponse:
    payload = {
        "login": data.login,
        "password_hash": data.password_hash,
        "public_key": data.public_key,
        "encrypted_private_key": data.encrypted_private_key,
    }

    async with httpx.AsyncClient() as client:
        response = await client.post(
            "https://digital-signature-auth.digitalsignature-auth.workers.dev/api/register",
            json=payload,
            headers={"Content-Type": "application/json"},
        )

    return RegisterNewUserResponse(status_code=response.status_code)


@router.post("/verify_user_login", response_model=VerifyUserLoginResponse)
async def verify_user_login(data: VerifyUserLogin) -> VerifyUserLoginResponse:
    payload = {
        "login": data.login,
        "password_hash": data.password_hash
    }
    
    async with httpx.AsyncClient() as client:
        response = await client.post(
            "https://digital-signature-auth.digitalsignature-auth.workers.dev/api/login",
            json=payload,
            headers={"Content-Type": "application/json"},
        )
        
    response_data = response.json()

    return VerifyUserLoginResponse(
        message=response_data.get("message"),
        encrypted_private_key=response_data.get("encrypted_private_key"),
        public_key=response_data.get("public_key")
    )
