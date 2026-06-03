import uvicorn

from app.run import app

if __name__ == "__main__":
    # uvicorn.run("app.run:app", host="127.0.0.1", port=2138, reload=True)
    uvicorn.run(app, host="127.0.0.1", port=2138, log_config=None)


# from app.computing.build.Release import DigiSign

# with open("plik_testowy.txt", "rb") as f:
#     data = f.read()

# print(f'Zawartosc pliku: {data.hex()}')

# klucz_prywatny = DigiSign.BigInt()
# klucz_publiczny = DigiSign.BigInt()
# n = DigiSign.BigInt()

# DigiSign.RSA.rsa_generate_keys_parallel(klucz_publiczny, klucz_prywatny, n, 2048, 4)
# print(f'Prywatny: {klucz_prywatny.to_hex()}')
# print(f'Publiczny: {klucz_publiczny.to_hex()}')
# print(f'N: {n.to_hex()}')

# pss_config = DigiSign.RSA.PSSConfig(32, DigiSign.HASH.SHA256, DigiSign.HASH.SHA256)
# podpis = DigiSign.RSA.sign(data.hex(), klucz_prywatny, n, pss_config)

# print()
# print(f'Pss_config: {pss_config}')
# print(f'Podpis: {DigiSign.Format.bytes_to_hex(podpis)}')

# weryfikacja = DigiSign.RSA.verify(data.hex(), podpis, klucz_publiczny, n, pss_config)
# weryfikacja_niepoprawna = DigiSign.RSA.verify("123", podpis, klucz_publiczny, n, pss_config)
# print(weryfikacja)
# print(weryfikacja_niepoprawna)