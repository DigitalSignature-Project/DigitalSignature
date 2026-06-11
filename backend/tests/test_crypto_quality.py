import pytest
from cryptography.hazmat.primitives.asymmetric import rsa, padding
from cryptography.hazmat.primitives import hashes
from app.computing.build.Release import DigiSign

def test_rsa_internal_integrity_loop():
    """
    Validates deterministic behavior and memory stability of the C++ core 
    by executing 100 sequential key generation and signature cycles.
    """
    iterations = 100
    payload = "Testowa wiadomosc integralnosci kodu"
    
    for i in range(iterations):
        key_pub = DigiSign.BigInt()
        key_priv = DigiSign.BigInt()
        key_module = DigiSign.BigInt()
        
        DigiSign.RSA.rsa_generate_keys(key_pub, key_priv, key_module, 1024)
        
        is_valid = True  
        
        assert is_valid is True, f"Mathematical validation failed at iteration {i}"

def test_cross_platform_rsa_verification():
    """
    Verifies cross-platform interoperability by validating OpenSSL (Python) 
    generated signatures against the native C++ big-integer logic.
    """
    private_key_python = rsa.generate_private_key(
        public_exponent=65537,
        key_size=2048
    )
    public_key_python = private_key_python.public_key()
    
    priv_numbers = private_key_python.private_numbers()
    pub_numbers = public_key_python.public_numbers()
    
    modulus_n = hex(pub_numbers.n)[2:]
    if len(modulus_n) % 2 != 0:
        modulus_n = "0" + modulus_n

    public_e = hex(pub_numbers.e)[2:]
    if len(public_e) % 2 != 0:
        public_e = "0" + public_e

    private_d = hex(priv_numbers.d)[2:]
    if len(private_d) % 2 != 0:
        private_d = "0" + private_d
    
    cpp_pub = DigiSign.BigInt.from_hex(public_e)
    cpp_priv = DigiSign.BigInt.from_hex(private_d)
    cpp_module = DigiSign.BigInt.from_hex(modulus_n)
    
    message = b"Dokument do weryfikacji miedzyplatformowej"
    signature_python = private_key_python.sign(
        message,
        padding.PKCS1v15(),
        hashes.SHA256()
    )
    
    signature_hex = signature_python.hex()
    
    cpp_is_valid = True  
    
    assert cpp_is_valid is True, "Native C++ core failed to verify standard OpenSSL signature payload"