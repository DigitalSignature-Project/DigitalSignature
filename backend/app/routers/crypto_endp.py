from fastapi import APIRouter, Depends

from app.auth import verify_token
from app.computing.build.Release import generate_rsa_key
from app.schemas.crypto_shemas import (
    RsaKeyResponse,
    RsaKeyParallelResponse,
    RsaKey,
    RsaKeyParallel,
)

router = APIRouter(dependencies=[Depends(verify_token)])


@router.post("/rsa_generate_keys", response_model=RsaKeyResponse)
async def rsa_generate_keys(data: RsaKey) -> RsaKeyResponse:
    key_pub: rsa_generate_keys.BigInt = generate_rsa_key.BigInt()
    key_priv: rsa_generate_keys.BigInt = generate_rsa_key.BigInt()
    key_module: rsa_generate_keys.BigInt = generate_rsa_key.BigInt()

    bits: int = data.bits

    generate_rsa_key.rsa_generate_keys(key_pub, key_priv, key_module, bits)

    # TODO
    print("key pub limbs")
    print(key_pub.limbs)
    print("key priv limnbs")
    print(key_priv.limbs)
    print("key_module")
    print(key_module.limbs)

    return RsaKeyResponse(
        key_pub="app works correctly",
        key_priv="app works correctly",
        key_module="app works correctly",
    )


@router.post("/rsa_generate_keys_parallel", response_model=RsaKeyParallelResponse)
async def rsa_generate_keys_parallel(data: RsaKeyParallel) -> RsaKeyParallelResponse:
    key_pub: rsa_generate_keys.BigInt = generate_rsa_key.BigInt()
    key_priv: rsa_generate_keys.BigInt = generate_rsa_key.BigInt()
    key_module: rsa_generate_keys.BigInt = generate_rsa_key.BigInt()

    bits: int = data.bits
    threads: int = data.threads

    generate_rsa_key.rsa_generate_keys_parallel(
        key_pub, key_priv, key_module, bits, threads
    )

    # TODO
    print("key pub limbs")
    print(key_pub.limbs)
    print("key priv limnbs")
    print(key_priv.limbs)
    print("key_module")
    print(key_module.limbs)

    return RsaKeyParallelResponse(
        key_pub="app works correctly P",
        key_priv="app works correctly P",
        key_module="app works correctly P",
    )
