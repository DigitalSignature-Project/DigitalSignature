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

    return RsaKeyResponse(
        key_pub=key_pub.to_hex(False),
        key_priv=key_priv.to_hex(False),
        key_module=key_module.to_hex(False),
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

    return RsaKeyParallelResponse(
        key_pub=key_pub.to_hex(False),
        key_priv=key_priv.to_hex(False),
        key_module=key_module.to_hex(False),
    )
