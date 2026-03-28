from fastapi import APIRouter
import time

from app.computing.build.Debug import generate_rsa_key

from app.schemas.crypto_shemas import (
    RsaKeyResponse,
    RsaKeyParallelResponse,
    DecryptResponse,
    EncryptResponse,
    RsaKey,
    RsaKeyParallel,
    Encrypt,
    Decrypt,
)

router = APIRouter()


@router.post("/rsa_generate_keys", response_model=RsaKeyResponse)
async def rsa_generate_keys(data: RsaKey) -> RsaKeyResponse:
    key_pub: rsa_generate_keys.BigInt = generate_rsa_key.BigInt()
    key_priv: rsa_generate_keys.BigInt = generate_rsa_key.BigInt()
    key_module: rsa_generate_keys.BigInt = generate_rsa_key.BigInt()

    bits: int = data.bits

    generate_rsa_key.rsa_generate_key(key_pub, key_priv, key_module, bits, 4)

    #TODO 
    print("key pub limbs")
    print(key_pub.limbs)
    print("key priv limnbs")
    print(key_priv.limbs)
    print("key_module")
    print(key_module.limbs)

    return RsaKeyResponse(key_pub="app works correctly", key_priv="app works correctly", key_module="app works correctly")

