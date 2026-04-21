import httpx
from fastapi import APIRouter, Depends

from app.core.encryption import encrypt, decrypt_bool
from app.auth import verify_token
from app.schemas.external_server_schemas import (
    RegisterNewUserResponse,
    RegisterNewUser,
    VerifyUserLogin,
    VerifyUserLoginResponse,
    RetrievePublicKeyResponse,
    KeyUpdate,
    KeyUpdateResponse,
    ServerStatusResponse,
)

router = APIRouter(dependencies=[Depends(verify_token)])


@router.post("/register_new_user", response_model=RegisterNewUserResponse)
async def register_new_user(data: RegisterNewUser) -> RegisterNewUserResponse:
    encrypted_private_key: str = encrypt(data.encrypted_private_key, data.private_key_user_password)
    
    payload: dict[str, str] = {
        "login": data.login,
        "password_hash": data.password_hash,
        "public_key": data.public_key,
        "encrypted_private_key": encrypted_private_key,
        "key_module": data.key_module,
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
    payload: dict[str, str] = {"login": data.login, "password_hash": data.password_hash}

    async with httpx.AsyncClient() as client:
        response = await client.post(
            "https://digital-signature-auth.digitalsignature-auth.workers.dev/api/login",
            json=payload,
            headers={"Content-Type": "application/json"},
        )

    response_data = response.json()

    return VerifyUserLoginResponse(
        message=response_data.get("message", "User not found"),
        encrypted_private_key=response_data.get("encrypted_private_key", ""),
        public_key=response_data.get("public_key", ""),
        key_module=response_data.get("key_module", ""),
    )


@router.get("/retrieve_public_key/{login}", response_model=RetrievePublicKeyResponse)
async def retrieve_public_key(login: str) -> RetrievePublicKeyResponse:
    async with httpx.AsyncClient() as client:
        response = await client.get(
            f"https://digital-signature-auth.digitalsignature-auth.workers.dev/api/public-key/{login}",
            headers={"Content-Type": "application/json"},
        )

    response_data = response.json()

    return RetrievePublicKeyResponse(
        login=response_data.get("login", ""), 
        public_key=response_data.get("public_key", ""),
        key_module=response_data.get("key_module", "")
    )


@router.post("/key_update", response_model=KeyUpdateResponse)
async def key_update(data: KeyUpdate) -> KeyUpdateResponse:
    payload: dict[str, str] = {
        "login": data.login,
        "password_hash": data.password_hash,
        "new_public_key": data.new_public_key,
        "new_encrypted_private_key": data.new_encrypted_private_key,
        "new_key_module": data.new_key_module,
    }

    async with httpx.AsyncClient() as client:
        response = await client.post(
            "https://digital-signature-auth.digitalsignature-auth.workers.dev/api/update-keys",
            json=payload,
            headers={"Content-Type": "application/json"},
        )

    return KeyUpdateResponse(status=response.status_code)


@router.get("/server_status", response_model=ServerStatusResponse)
async def server_status() -> ServerStatusResponse:
    async with httpx.AsyncClient() as client:
        response = await client.get(
            "https://digital-signature-auth.digitalsignature-auth.workers.dev/api/status",
            headers={"Content-Type": "application/json"},
        )

    return ServerStatusResponse(status=response.status_code)
