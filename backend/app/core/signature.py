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
            f"https://digital-signature-auth.digitalsignature-auth.workers.dev/api/public-keys/{login}",
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


async def generate_elgamal_data() -> dict:
    key_pub = DigiSign.ElGamal.ElGamalPublicKey()
    key_priv = DigiSign.BigInt()

    DigiSign.ElGamal.ElGamal_generate_keys_parallel(key_pub, key_priv, 2048, 256, 4)

    result: dict = {
        "p_hex": key_pub.p.to_hex(),
        "q_hex": key_pub.q.to_hex(),
        "g_hex": key_pub.g.to_hex(),
        "y_hex": key_pub.y.to_hex(),
        "key_priv_hex": key_priv.to_hex(),
    }

    return result


async def generate_ecdsa_data() -> dict:
    key_pub = DigiSign.ECDSA.PublicKey()
    key_priv = DigiSign.BigInt()

    DigiSign.ECDSA.ecdsa_generate_keys(key_pub, key_priv)

    result: dict = {
        "x_hex": key_pub.x.to_hex(),
        "y_hex": key_pub.y.to_hex(),
        "key_priv_hex": key_priv.to_hex(),
    }

    return result


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


async def create_elgamal_signature(
    file_content: str, login: str, hash: str, password: str
) -> str:
    match hash:
        case "SHA256":
            hash_1 = DigiSign.HASH.SHA256
        case "SHA3_256":
            hash_1 = DigiSign.HASH.SHA3_256
        case "SHA3_512":
            hash_1 = DigiSign.HASH.SHA3_512
        case _:
            raise ValueError("Unsupported hash function")

    async with httpx.AsyncClient() as client:
        response = await client.get(
            f"https://digital-signature-auth.digitalsignature-auth.workers.dev/api/public-keys/{login}",
            headers={"Content-Type": "application/json"},
        )

    data = response.json()

    p = data["elgamal_keys"][0]["p_value"]
    q = data["elgamal_keys"][0]["q_value"]
    g = data["elgamal_keys"][0]["g_value"]
    y = data["elgamal_keys"][0]["y_value"]
    encrypted_private_key = data["elgamal_keys"][0]["encrypted_private_key"]

    try:
        private_key = decrypt(encrypted_private_key, password)
    except InvalidTag:
        raise HTTPException(status_code=401, detail="Invalid key passphrase")

    p = DigiSign.BigInt.from_hex(p)
    q = DigiSign.BigInt.from_hex(q)
    g = DigiSign.BigInt.from_hex(g)
    y = DigiSign.BigInt.from_hex(y)

    public_key = DigiSign.ElGamal.ElGamalPublicKey(p, q, g, y)
    private_key = DigiSign.BigInt.from_hex(private_key)
    signature = DigiSign.ElGamal.sign(file_content, public_key, private_key, hash_1)
    signature = DigiSign.ElGamal.DER_encode_signature_hex(signature)
    return signature


async def verify_elgamal_sign(
    file_content: str, signature: str, login: str, hash: str
) -> bool:
    match hash:
        case "SHA256":
            hash_1 = DigiSign.HASH.SHA256
        case "SHA3_256":
            hash_1 = DigiSign.HASH.SHA3_256
        case "SHA3_512":
            hash_1 = DigiSign.HASH.SHA3_512
        case _:
            raise ValueError("Unsupported hash function")

    async with httpx.AsyncClient() as client:
        response = await client.get(
            f"https://digital-signature-auth.digitalsignature-auth.workers.dev/api/public-keys/{login}",
            headers={"Content-Type": "application/json"},
        )

    data = response.json()

    p = data["elgamal_keys"][0]["p_value"]
    q = data["elgamal_keys"][0]["q_value"]
    g = data["elgamal_keys"][0]["g_value"]
    y = data["elgamal_keys"][0]["y_value"]

    p = DigiSign.BigInt.from_hex(p)
    q = DigiSign.BigInt.from_hex(q)
    g = DigiSign.BigInt.from_hex(g)
    y = DigiSign.BigInt.from_hex(y)

    public_key = DigiSign.ElGamal.ElGamalPublicKey(p, q, g, y)

    sign_class = DigiSign.ElGamal.DER_decode_signature(signature)
    result = DigiSign.ElGamal.verify(file_content, public_key, sign_class, hash_1)
    return result


async def create_escda_signature(
    file_content: str, login: str, password: str, hash: str
) -> str:
    match hash:
        case "SHA256":
            hash_1 = DigiSign.HASH.SHA256
        case "SHA3_256":
            hash_1 = DigiSign.HASH.SHA3_256
        case "SHA3_512":
            hash_1 = DigiSign.HASH.SHA3_512
        case _:
            raise ValueError("Unsupported hash function")

    async with httpx.AsyncClient() as client:
        response = await client.get(
            f"https://digital-signature-auth.digitalsignature-auth.workers.dev/api/public-keys/{login}",
            headers={"Content-Type": "application/json"},
        )

    data = response.json()

    encrypted_private_key = data["ecdsa_keys"][0]["encrypted_private_key"]

    try:
        private_key = decrypt(encrypted_private_key, password)
    except InvalidTag:
        raise HTTPException(status_code=401, detail="Invalid key passphrase")

    private_key = DigiSign.BigInt.from_hex(private_key)
    signature = DigiSign.ECDSA.sign(file_content, private_key, hash_1)
    signature = DigiSign.ECDSA.DER_encode_signature_hex(signature)
    return signature


async def verify_escda_sign(
    file_content: str, signature: str, login: str, hash: str
) -> bool:
    match hash:
        case "SHA256":
            hash_1 = DigiSign.HASH.SHA256
        case "SHA3_256":
            hash_1 = DigiSign.HASH.SHA3_256
        case "SHA3_512":
            hash_1 = DigiSign.HASH.SHA3_512
        case _:
            raise ValueError("Unsupported hash function")

    async with httpx.AsyncClient() as client:
        response = await client.get(
            f"https://digital-signature-auth.digitalsignature-auth.workers.dev/api/public-keys/{login}",
            headers={"Content-Type": "application/json"},
        )

    data = response.json()

    x_value = data["ecdsa_keys"][0]["x_value"]
    y_value = data["ecdsa_keys"][0]["y_value"]

    x_value = DigiSign.BigInt.from_hex(x_value)
    y_value = DigiSign.BigInt.from_hex(y_value)

    sign_class = DigiSign.ECDSA.DER_decode_signature(signature)
    public_key = DigiSign.ECDSA.PublicKey(x_value, y_value)
    result = DigiSign.ECDSA.verify(file_content, public_key, sign_class, hash_1)
    return result
