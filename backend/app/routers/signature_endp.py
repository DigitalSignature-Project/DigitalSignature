from fastapi import APIRouter, Depends, HTTPException

from app.auth import verify_token
from app.schemas.signature_schemas import RsaSignatureResponse, RsaSignature, VerifyRsaSignature, VerifyRsaSignatureResponse
from app.core.signature import create_rsa_signature, verify_rsa_signature


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
        data.hash_function_2
    )
    return RsaSignatureResponse(signature=signature)


@router.post("/verify_rsa_signature", response_model=VerifyRsaSignatureResponse)
async def verify_rsa_sign(data: VerifyRsaSignature) -> VerifyRsaSignatureResponse:
    result = await verify_rsa_signature(data.file_content, data.signature, data.login, data.salt_length, data.hash_function_1, data.hash_function_2)
    return VerifyRsaSignatureResponse(is_valid=result)
    