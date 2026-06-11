from fastapi import APIRouter, Depends, HTTPException

from app.auth import verify_token
from app.schemas.signature_schemas import (
    RsaSignatureResponse,
    RsaSignature,
    VerifyRsaSignature,
    VerifyRsaSignatureResponse,
    ElgamalSignatureResponse,
    ElgamalSignature,
    VerifyElgamalSignatureResponse,
    VerifyElgamalSignature
)
from app.core.signature import create_rsa_signature, verify_rsa_signature, create_elgamal_signature, verify_elgamal_sign

router = APIRouter(dependencies=[Depends(verify_token)])


@router.post("/generate_rsa_signature", response_model=RsaSignatureResponse)
async def generate_rsa_signature(data: RsaSignature) -> RsaSignatureResponse:
    signature = await create_rsa_signature(
        data.file_content,
        data.login,
        data.password,
        data.encrypted_private_key,
        data.key_module,
        data.salt_length,
        data.hash_function_1,
        data.hash_function_2,
    )
    return RsaSignatureResponse(signature=signature)


@router.post("/verify_rsa_signature", response_model=VerifyRsaSignatureResponse)
async def verify_rsa_sign(data: VerifyRsaSignature) -> VerifyRsaSignatureResponse:
    result = await verify_rsa_signature(
        data.file_content,
        data.signature,
        data.login,
        data.salt_length,
        data.hash_function_1,
        data.hash_function_2,
    )
    return VerifyRsaSignatureResponse(is_valid=result)


@router.post("/generate_elgamal_signature", response_model=ElgamalSignatureResponse)
async def generate_elgamal_signature(data: ElgamalSignature) -> ElgamalSignatureResponse:
    signature = await create_elgamal_signature(
        data.file_content,
        data.login,
        data.hash
    )
    return ElgamalSignatureResponse(signature=signature)


@router.post("/verify_elgamal_signature", response_model=VerifyElgamalSignatureResponse)
async def verify_elgamal_signature(data: VerifyElgamalSignature) -> VerifyElgamalSignatureResponse:
    result = await verify_elgamal_sign(
        data.file_content,
        data.signature,
        data.login,
        data.hash
    )
    return VerifyElgamalSignatureResponse(is_valid=result)
