import uvicorn

from app.run import app

if __name__ == "__main__":
    # uvicorn.run("app.run:app", host="127.0.0.1", port=2138, reload=True)
    uvicorn.run(app, host="127.0.0.1", port=2138, log_config=None)


# from app.computing.build.Release import DigiSign

# wiadomosc = "wiadomosc testowa"

# key_pub = DigiSign.BigInt()
# key_priv = DigiSign.BigInt()
# key_module = DigiSign.BigInt()

# bits: int = 1024

# DigiSign.RSA.rsa_generate_keys(key_pub, key_priv, key_module, bits)

# pss_config = DigiSign.RSA.PSSConfig(35, hash_function = DigiSign.HASH.SHA256, MGF1 = DigiSign.HASH.SHA256)
# podpis = DigiSign.RSA.sign(wiadomosc, key_priv, key_module, pss_config)

# weryfikacja = DigiSign.RSA.verify(wiadomosc, podpis, key_pub, key_module, pss_config)

# if weryfikacja:
#     print("podpis prawidlowy")
# else:
#     print("podpis nieprawidlowy")


# from cryptography.hazmat.primitives.asymmetric import rsa, padding
# from cryptography.hazmat.primitives import hashes


# n = int(key_module.to_hex(False), 16)
# e = 65537

# signature = bytes.fromhex(DigiSign.Format.bytes_to_hex(podpis))
# message = b"wiadomosc testowa"
# public_key = rsa.RSAPublicNumbers(e, n).public_key()

# try:
#     public_key.verify(
#         signature,
#         message,
#         padding.PSS(
#             mgf=padding.MGF1(hashes.SHA256()),
#             salt_length=35
#         ),
#         hashes.SHA256()
#     )
#     print("VALID")
# except Exception as ex:
#     print("INVALID")
#     print(ex)
