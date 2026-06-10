import httpx
from cryptography.exceptions import InvalidTag
from fastapi import HTTPException

from app.computing.build.Release import DigiSign
from app.core.encryption import decrypt


def _convert_parameters(
    salt_length: str, hash_function_1: str, hash_function_2: str
) -> tuple[str, str, str]:
    try:
        salt_length = int(salt_length)
    except ValueError:
        raise HTTPException(status_code=400, detail="salt_length must be an integer")

    match hash_function_1:
        case "SHA256":
            hash_1 = DigiSign.HASH.SHA256
        case "SHA3_256":
            hash_1 = DigiSign.HASH.SHA3_256
        case "SHA3_512":
            hash_1 = DigiSign.HASH.SHA3_512
        case _:
            raise ValueError("Unsupported hash function")

    match hash_function_2:
        case "SHA256":
            hash_2 = DigiSign.HASH.SHA256
        case "SHA3_256":
            hash_2 = DigiSign.HASH.SHA3_256
        case "SHA3_512":
            hash_2 = DigiSign.HASH.SHA3_512
        case _:
            raise ValueError("Unsupported hash function")

    return salt_length, hash_1, hash_2


async def _fetch_user_keys(login: str) -> dict:
    async with httpx.AsyncClient() as client:
        response = await client.get(
            f"https://digital-signature-auth.digitalsignature-auth.workers.dev/api/public-key/{login}",
            headers={"Content-Type": "application/json"},
        )

    if response.status_code == 404:
        raise HTTPException(status_code=404, detail="User not found")

    response.raise_for_status()
    response_data = response.json()
    encrypted_private_key = response_data.get("encrypted_private_key", "")
    key_module = response_data.get("key_module", "")

    if not encrypted_private_key or not key_module:
        raise HTTPException(
            status_code=400,
            detail="Missing user keys. Log in again to refresh session data.",
        )

    return {
        "encrypted_private_key": encrypted_private_key,
        "key_module": key_module,
    }


async def create_rsa_signature(
    file_content: str,
    login: str,
    password: str,
    encrypted_private_key: str,
    key_module: str,
    salt_length: str,
    hash_function_1: str,
    hash_function_2: str,
) -> str:
    salt_length, hash_1, hash_2 = _convert_parameters(
        salt_length, hash_function_1, hash_function_2
    )
    pss_config = DigiSign.RSA.PSSConfig(salt_length, hash_1, hash_2)

    if not encrypted_private_key or not key_module:
        keys = await _fetch_user_keys(login)
        encrypted_private_key = keys["encrypted_private_key"]
        key_module = keys["key_module"]

    try:
        private_key_hex = decrypt(encrypted_private_key, password)
    except InvalidTag:
        raise HTTPException(status_code=401, detail="Invalid key passphrase")

    private_key = DigiSign.BigInt.from_hex(private_key_hex)
    key_module_int = DigiSign.BigInt.from_hex(key_module)
    signature = DigiSign.RSA.sign(file_content, private_key, key_module_int, pss_config)
    return DigiSign.Format.bytes_to_hex(signature)


async def verify_rsa_signature(
    file_content: str,
    signature: str,
    login: str,
    salt_length: str,
    hash_function_1: str,
    hash_function_2: str,
) -> bool:
    salt_length, hash_1, hash_2 = _convert_parameters(
        salt_length, hash_function_1, hash_function_2
    )
    pss_config = DigiSign.RSA.PSSConfig(salt_length, hash_1, hash_2)

    async with httpx.AsyncClient() as client:
        response = await client.get(
            f"https://digital-signature-auth.digitalsignature-auth.workers.dev/api/public-keys/{login}",
            headers={"Content-Type": "application/json"},
        )

    response_data = response.json()

    public_key = DigiSign.BigInt.from_hex(response_data.get("public_key", ""))
    key_module = DigiSign.BigInt.from_hex(response_data.get("key_module", ""))
    result = DigiSign.RSA.verify(
        file_content, signature, public_key, key_module, pss_config
    )
    return result
