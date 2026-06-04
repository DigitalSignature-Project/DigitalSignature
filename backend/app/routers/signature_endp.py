from fastapi import APIRouter, Depends, HTTPException

from app.auth import verify_token
from app.schemas.signature_schemas import RsaSignatureResponse, RsaSignature, VerifyRsaSignature, VerifyRsaSignatureResponse
from app.core.signature import create_rsa_signature, verify_rsa_signature


router = APIRouter(dependencies=[Depends(verify_token)])


@router.post("/generate_rsa_signature", response_model=RsaSignatureResponse)
async def generate_rsa_signature(data: RsaSignature) -> RsaSignatureResponse:
    # file_content = data.file_content
    # login = data.login
    # password = data.password

    # signature = await create_rsa_signature(file_content, login, password)
    # return RsaSignatureResponse(signature=signature)
    return RsaSignatureResponse(signature="test")


@router.post("/verify_rsa_signature", response_model=VerifyRsaSignatureResponse)
async def verify_rsa_sign(data: VerifyRsaSignature) -> VerifyRsaSignatureResponse:
    file_content = data.file_content
    signature = data.signature
    login = data.login

    result = await verify_rsa_signature(file_content, signature, login)
    return VerifyRsaSignatureResponse(is_valid=result)
    