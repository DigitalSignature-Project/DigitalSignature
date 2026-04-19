# import uvicorn

# from app.run import app

# if __name__ == "__main__":
#     # uvicorn.run("app.run:app", host="127.0.0.1", port=2138, reload=True)
#     uvicorn.run(app, host="127.0.0.1", port=2138, log_config=None)


import httpx
import asyncio

async def register_new_user():
    response_register_new_user = httpx.post(
        "https://digital-signature-auth.digitalsignature-auth.workers.dev/api/register",
        json={
            "login": "johndoe12",
            "password_hash": "hashed_password_string",
            "public_key": "public_key_string",
            "encrypted_private_key": "aes_encrypted_private_key_blob"
        },
        headers={"Content-Type": "application/json"}
    )
    
    print("Pierwszy endpoint")
    print(response_register_new_user.status_code)
    print(response_register_new_user.text)


async def register_login_verifies():
    response_login_verifies = httpx.post(
        "https://digital-signature-auth.digitalsignature-auth.workers.dev/api/login",
        json={
            "login": "johndoe1",
            "password_hash": "hashed_password_string"
        },
        headers={"Content-Type": "application/json"}
    )
    
    print("Drugi endpoint")
    print(response_login_verifies.status_code)
    print(response_login_verifies.text)
    
    
async def retrieving_public_key(user: str):
    async with httpx.AsyncClient() as client:
        response = await client.get(
            f"https://digital-signature-auth.digitalsignature-auth.workers.dev/api/public-key/{user}"
        )

        print("Trzeci endpoint")
        print(response.status_code)
        print(response.text)


async def key_update():
    response_key_update = httpx.post(
        "https://digital-signature-auth.digitalsignature-auth.workers.dev/api/update-keys",
        json={
            "login": "johndoe",
            "password_hash": "hashed_password_string",
            "new_public_key": "new_public_key_string",
            "new_encrypted_private_key": "new_aes_encrypted_private_key_blob"
        },
        headers={"Content-Type": "application/json"}
    )
    
    print("Czwarty endpoint")
    print(response_key_update.status_code)
    print(response_key_update.text)


async def check_server_status():
    async with httpx.AsyncClient() as client:
        response = await client.get(
            "https://digital-signature-auth.digitalsignature-auth.workers.dev/api/status"
        )

        print("Piaty endpoint")
        print(response.status_code)
        print(response.text)
        

asyncio.run(register_new_user())
asyncio.run(register_login_verifies())
asyncio.run(retrieving_public_key("johndoe1"))
asyncio.run(key_update())
asyncio.run(check_server_status())


# from app.computing.build.Release import generate_rsa_key

# wiadomosc = "wiadomosc testowa"

# key_pub = generate_rsa_key.BigInt()
# key_priv = generate_rsa_key.BigInt()
# key_module = generate_rsa_key.BigInt()

# bits: int = 1024

# generate_rsa_key.rsa_generate_keys(key_pub, key_priv, key_module, bits)

# podpis = generate_rsa_key.pss_encode(wiadomosc, int(bits / 8), key_module)
# hasz = generate_rsa_key.bytes_to_hex(podpis)
# big_int_podpis = generate_rsa_key.vectoruint8(podpis)
# zaszyfrowany = generate_rsa_key.encrypt(big_int_podpis, key_priv, key_module)
# wektor_zaszyfrowany = zaszyfrowany.to_vectoruint8()
# weryfikacja = generate_rsa_key.verify(wiadomosc, wektor_zaszyfrowany, key_pub, key_module)

# if weryfikacja:
#     print("podpis prawidlowy")
# else:
#     print("podpis nieprawidlowy")


# from cryptography.hazmat.primitives.asymmetric import rsa, padding
# from cryptography.hazmat.primitives import hashes


# n = int(key_module.to_hex(False), 16)
# e = 65537

# signature = bytes.fromhex(generate_rsa_key.bytes_to_hex(wektor_zaszyfrowany))
# message = b"wiadomosc testowa"
# public_key = rsa.RSAPublicNumbers(e, n).public_key()

# try:
#     public_key.verify(
#         signature,
#         message,
#         padding.PSS(
#             mgf=padding.MGF1(hashes.SHA256()),
#             salt_length=32
#         ),
#         hashes.SHA256()
#     )
#     print("VALID")
# except Exception as ex:
#     print("INVALID")
#     print(ex)
