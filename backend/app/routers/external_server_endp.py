import httpx
from fastapi import APIRouter, Depends, HTTPException

from app.computing.build.Release import DigiSign
from app.core.encryption import encrypt, decrypt_bool
from app.auth import verify_token
from app.core.signature import generate_ecdsa_data, generate_elgamal_data
from app.schemas.external_server_schemas import (
    RegisterNewUserResponse,
    RegisterNewUser,
    VerifyUserLogin,
    VerifyUserLoginResponse,
    RetrievePublicKeyResponse,
    KeyUpdate,
    KeyUpdateResponse,
    ServerStatusResponse,
    CheckUserKey,
    CheckUserKeyResponse,
)

router = APIRouter(dependencies=[Depends(verify_token)])


@router.post("/register_new_user", response_model=RegisterNewUserResponse)
async def register_new_user(data: RegisterNewUser) -> RegisterNewUserResponse:
    encrypted_private_key: str = encrypt(
        data.encrypted_private_key, data.private_key_user_password
    )

    hashed_password = DigiSign.HASH.SHA256(data.password_hash)

    payload: dict[str, str] = {
        "login": data.login,
        "password_hash": hashed_password,
        "public_key": data.public_key,
        "encrypted_private_key": encrypted_private_key,
        "key_module": data.key_module,
    }
    
    elgamal_data = await generate_elgamal_data()
    
    elgamal_payload: dict[str, str] = {
        "login": data.login,
        "password_hash": hashed_password,
        "key_type": "elgamal",
        "p_value": elgamal_data["p_hex"],
        "q_value": elgamal_data["q_hex"],
        "g_value": elgamal_data["g_hex"],
        "y_value": elgamal_data["y_hex"],
        "encrypted_private_key": elgamal_data["key_priv_hex"]
    }
    
    ecdsa_data = await generate_ecdsa_data()
    
    ecdsa_payload: dict[str, str] = {
        "login": data.login,
        "password_hash": hashed_password,
        "key_type": "ecdsa",
        "x_value": ecdsa_data["x_hex"],
        "y_value": ecdsa_data["y_hex"],
        "encrypted_private_key": ecdsa_data["key_priv_hex"]
    }

    async with httpx.AsyncClient() as client:
        response_rsa_user = await client.post(
            "https://digital-signature-auth.digitalsignature-auth.workers.dev/api/register",
            json=payload,
            headers={"Content-Type": "application/json"},
        )
        
    async with httpx.AsyncClient() as client:
        response_elgamal = await client.post(
            "https://digital-signature-auth.digitalsignature-auth.workers.dev/api/add-key",
            json=elgamal_payload,
            headers={"Content-Type": "application/json"},
        )
        
    async with httpx.AsyncClient() as client:
        response_ecdsa = await client.post(
            "https://digital-signature-auth.digitalsignature-auth.workers.dev/api/add-key",
            json=ecdsa_payload,
            headers={"Content-Type": "application/json"},
        )
        
    if response_rsa_user.status_code != 201 and response_rsa_user.status_code != 200:
        return RegisterNewUserResponse(status_code=response_rsa_user.status_code) 
    
    if response_elgamal.status_code != 201 and response_elgamal.status_code != 200:
        return RegisterNewUserResponse(status_code=response_elgamal.status_code) 
    
    if response_ecdsa.status_code != 201 and response_ecdsa.status_code != 200:
        return RegisterNewUserResponse(status_code=response_ecdsa.status_code) 

    return RegisterNewUserResponse(status_code=response_rsa_user.status_code)


@router.post("/verify_user_login", response_model=VerifyUserLoginResponse)
async def verify_user_login(data: VerifyUserLogin) -> VerifyUserLoginResponse:
    hashed_password = DigiSign.HASH.SHA256(data.password_hash)
    payload: dict[str, str] = {"login": data.login, "password_hash": hashed_password}

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
            f"https://digital-signature-auth.digitalsignature-auth.workers.dev/api/public-keys/{login}",
            headers={"Content-Type": "application/json"},
        )

    response_data = response.json()

    return RetrievePublicKeyResponse(
        login=response_data.get("login", ""),
        public_key=response_data.get("public_key", ""),
        key_module=response_data.get("key_module", ""),
    )


@router.post("/key_update", response_model=KeyUpdateResponse)
async def key_update(data: KeyUpdate) -> KeyUpdateResponse:
    hashed_password = DigiSign.HASH.SHA256(data.password_hash)

    payload: dict[str, str] = {
        "login": data.login,
        "password_hash": hashed_password,
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


@router.post("/check_user_key", response_model=CheckUserKeyResponse)
async def check_user_key(data: CheckUserKey) -> CheckUserKeyResponse:
    hashed_password = DigiSign.HASH.SHA256(data.password_hash)
    payload: dict[str, str] = {"login": data.login, "password_hash": hashed_password}

    async with httpx.AsyncClient() as client:
        response = await client.post(
            "https://digital-signature-auth.digitalsignature-auth.workers.dev/api/login",
            json=payload,
            headers={"Content-Type": "application/json"},
        )

    response_data = response.json()
    decrypt_status: bool = decrypt_bool(
        response_data.get("encrypted_private_key", ""), data.key
    )

    if decrypt_status:
        return CheckUserKeyResponse(status=200)
    raise HTTPException(status_code=401, detail="Invalid key")
