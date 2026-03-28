from fastapi import APIRouter
import time

import sys
sys.path.append(r"app/computing/build/Debug") 
import generate_rsa_key

from app.schemas.bmi_schemas import Bmi, BmiResult
#from app.computing.dist import bmi_computing

router = APIRouter()

"""
public_key = generate_rsa_key.BigInt()
private_key = generate_rsa_key.BigInt()
n = generate_rsa_key.BigInt()

generate_rsa_key.rsa_generate_keys(public_key, private_key, n, 512)

print(private_key.limbs)

message = 2137

print(message)

encrypted_message = generate_rsa_key.encrypt(message, public_key, n)

print(encrypted_message.limbs)

decrypted_message = generate_rsa_key.decrypt(encrypted_message, private_key, n)

print(decrypted_message)
"""

@router.post("/bmi", response_model=BmiResult)
async def calculate_bmi(data: Bmi) -> BmiResult:
    # bmi_value = bmi_computing.calculate_bmi(data.height, data.weight)
    # bmi_value = bmi_computing.add_two_numbers(data.height, data.weight)
    #bmi_value = generate_rsa_key.rsa_generate_keys()

    public_key = generate_rsa_key.BigInt()
    private_key = generate_rsa_key.BigInt()
    n = generate_rsa_key.BigInt()


    start = time.time()
    generate_rsa_key.RSA_generate_keys_parallel(public_key, private_key, n, 2048, 4)
    end = time.time()

    print(end-start)

    start = time.time()
    generate_rsa_key.rsa_generate_keys(public_key, private_key, n, 2048)
    end = time.time()

    print(end-start)

    print(private_key.limbs)

    message = 2137

    print(message)

    encrypted_message = generate_rsa_key.encrypt(message, public_key, n)

    print(encrypted_message.limbs)

    decrypted_message = generate_rsa_key.decrypt(encrypted_message, private_key, n)

    print(decrypted_message)
    return BmiResult(result=decrypted_message)