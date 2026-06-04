import httpx

from app.computing.build.Release import DigiSign
from app.core.encryption import decrypt


async def create_rsa_signature(file_content: str, login: str, password: str) -> str:
    # pss_config = DigiSign.RSA.PSSConfig(32, DigiSign.HASH.SHA256, DigiSign.HASH.SHA256)

    # async with httpx.AsyncClient() as client:
    #     response = await client.get(
    #         f"https://digital-signature-auth.digitalsignature-auth.workers.dev/api/public-key/{login}",
    #         headers={"Content-Type": "application/json"},
    #     )
    
    # response_data = response.json()

    # private_key = DigiSign.BigInt.from_hex(response_data.get("encrypted_private_key", ""))
    # private_key = decrypt(private_key, password)
    # key_module = DigiSign.BigInt.from_hex(response_data.get("key_module", ""))
    # signature = DigiSign.RSA.sign(file_content, private_key, key_module, pss_config)
    # return DigiSign.Format.bytes_to_hex(signature)
    return "test"


async def verify_rsa_signature(file_content: str, signature: str, login: str) -> bool:
    pss_config = DigiSign.RSA.PSSConfig(32, DigiSign.HASH.SHA256, DigiSign.HASH.SHA256)

    async with httpx.AsyncClient() as client:
        response = await client.get(
            f"https://digital-signature-auth.digitalsignature-auth.workers.dev/api/public-key/{login}",
            headers={"Content-Type": "application/json"},
        )

    response_data = response.json()

    public_key = DigiSign.BigInt.from_hex(response_data.get("public_key", ""))
    key_module = DigiSign.BigInt.from_hex(response_data.get("key_module", ""))
    result = DigiSign.RSA.verify(file_content, signature, public_key, key_module, pss_config)
    return result
