import uvicorn

if __name__ == "__main__":
    uvicorn.run("app.run:app", host="127.0.0.1", port=2138, reload=True)


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
