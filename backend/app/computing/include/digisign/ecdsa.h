#pragma once
#include <string>
#include <functional>
#include <digisign/BigInt.h>

namespace digisign {

/**
 * @brief Affine point on the elliptic curve.
 *
 * Represents a point (x, y) on the secp256k1 curve in affine coordinates.
 * Infinity is represented by the boolean flag @ref infinity.
 *
 * Used for public keys, intermediate results, and final outputs of scalar multiplication.
 */
struct CurvePoint;

/**
 * @brief secp256k1 curve parameters and modular arithmetic helpers.
 *
 * Curve equation:
 *   y² = x³ + 7  (mod p)
 *
 * Parameters:
 *   - p : field modulus
 *   - a : curve coefficient (0 for secp256k1)
 *   - b : curve coefficient (7 for secp256k1)
 *   - n : order of generator point G
 *   - G : generator point
 *
 * Includes Montgomery helpers for fast modular arithmetic.
 */
struct Curve {

    // ===== secp256k1 parameters =====

    static BigInt p; ///< Field modulus
    static BigInt a; ///< Curve coefficient a = 0
    static BigInt b; ///< Curve coefficient b = 7
    static BigInt n; ///< Order of generator point G
    static CurvePoint G; ///< Generator point

    /**
     * @brief Check whether (x, y) lies on the curve.
     *
     * @param x X coordinate
     * @param y Y coordinate
     * @return true if (x, y) satisfies the curve equation.
     */
    static bool contains(const BigInt& x, const BigInt& y);

    // ===== Montgomery helpers for mod p =====

    /**
     * @brief Precomputed Montgomery inverse of p.
     */
    static uint64_t p_inv();

    /**
     * @brief Precomputed R² mod p for Montgomery multiplication.
     */
    static const BigInt& p_R2();

    // ===== Montgomery helpers for mod n =====

    /**
     * @brief Precomputed Montgomery inverse of n.
     */
    static uint64_t nc_inv();

    /**
     * @brief Precomputed R² mod n for Montgomery multiplication.
     */
    static const BigInt& n_R2();

    /**
     * @brief Convert integer to Montgomery domain modulo p.
     */
    static BigInt to_mont(const BigInt& a);

    /**
     * @brief Convert integer from Montgomery domain modulo p.
     */
    static BigInt from_mont(const BigInt& aM);

    /**
     * @brief Modular addition modulo p.
     */
    static BigInt mod_add_p(const BigInt& a, const BigInt& b);

    /**
     * @brief Modular subtraction modulo p.
     */
    static BigInt mod_sub_p(const BigInt& a, const BigInt& b);

    /**
     * @brief Montgomery modular multiplication modulo p.
     */
    static BigInt mod_mul_p(const BigInt& a, const BigInt& b);
};

/**
 * @brief Point in Jacobian coordinates for fast EC arithmetic.
 *
 * Representation:
 *   (X : Y : Z) corresponds to affine (X/Z², Y/Z³).
 *
 * Used internally for scalar multiplication and point addition.
 */
struct JacobianPoint {

    BigInt X; ///< X coordinate in Jacobian form
    BigInt Y; ///< Y coordinate in Jacobian form
    BigInt Z; ///< Z coordinate in Jacobian form
    bool infinity; ///< True if point is the point at infinity

    JacobianPoint();
    JacobianPoint(const BigInt& X, const BigInt& Y, const BigInt& Z);

    /**
     * @brief Convert Jacobian point to affine coordinates.
     *
     * @return Affine point (x, y).
     */
    CurvePoint to_affine() const;

    /**
     * @brief Point doubling in Jacobian coordinates.
     */
    JacobianPoint dbl() const;

    /**
     * @brief Point addition in Jacobian coordinates.
     */
    JacobianPoint operator+(const JacobianPoint& Q) const;

    /**
     * @brief Scalar multiplication using double-and-add.
     *
     * @param k Scalar multiplier.
     * @return k * this (in Jacobian form).
     */
    JacobianPoint operator*(const BigInt& k) const;
};

/**
 * @brief Affine point on the curve.
 *
 * Used for public keys and final results of scalar multiplication.
 */
struct CurvePoint {

    BigInt x; ///< X coordinate
    BigInt y; ///< Y coordinate
    bool infinity; ///< True if point is the point at infinity

    CurvePoint();
    CurvePoint(const BigInt& x, const BigInt& y);

    /**
     * @brief Convert affine point to Jacobian coordinates.
     */
    JacobianPoint to_jacobian() const;

    /**
     * @brief Add two affine points.
     */
    CurvePoint operator+(const CurvePoint& Q) const;

    /**
     * @brief Point doubling in affine coordinates.
     */
    CurvePoint dbl() const;

    /**
     * @brief Scalar multiplication in affine coordinates.
     *
     * Internally converts to Jacobian and back.
     */
    CurvePoint operator*(const BigInt& k) const;
};

/**
 * @brief ECDSA public key.
 *
 * Contains a single affine point Q = dG.
 */
struct ECDSAPublicKey {

    CurvePoint key_public; ///< Public key point

    ECDSAPublicKey();
    ECDSAPublicKey(const CurvePoint& key_pub);
};

/**
 * @brief ECDSA signature (r, s).
 *
 * Both values lie in [1, n−1].
 */
struct ECDSASignature {

    BigInt r; ///< First signature component
    BigInt s; ///< Second signature component

    ECDSASignature();
    ECDSASignature(const BigInt& r, const BigInt& s);
};

/**
 * @brief Generate ECDSA keypair.
 *
 * Private key:
 *   d ∈ [1, n−1]
 *
 * Public key:
 *   Q = dG
 *
 * @param key_pub Output: public key Q.
 * @param key_priv Output: private key d.
 */
void ECDSA_generate_keys(ECDSAPublicKey& key_pub, BigInt& key_priv);

/**
 * @brief Sign a message using ECDSA.
 *
 * Signature equations:
 *   r = (kG).x mod n
 *   s = k^{-1}(z + r d) mod n
 *
 * Hash z is truncated to n_bits (DSA-style hash_to_int).
 *
 * @param message Message to sign.
 * @param key_priv Private key d.
 * @param hash_function Hash function H: bytes → bytes.
 * @return ECDSA signature (r, s).
 */
ECDSASignature ecdsa_sign(const std::string& message,
                          const BigInt& key_priv,
                          std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function);

/**
 * @brief Verify an ECDSA signature.
 *
 * Verification equations:
 *   w  = s^{-1} mod n
 *   u1 = z w mod n
 *   u2 = r w mod n
 *   P  = u1 G + u2 Q
 * Signature valid iff:
 *   P.x mod n == r
 *
 * @param message Message to verify.
 * @param key_pub Public key Q.
 * @param signature Signature (r, s).
 * @param hash_function Hash function H.
 * @return true if signature is valid.
 */
bool ecdsa_verify(const std::string& message,
                  const ECDSAPublicKey& key_pub,
                  const ECDSASignature& signature,
                  std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function);

/**
 * @brief Verify a hex-encoded DER signature.
 */
bool ecdsa_verify(const std::string& message,
                  const ECDSAPublicKey& key_pub,
                  const std::string& hex_signature,
                  std::function<std::vector<uint8_t>(const std::vector<uint8_t>&)> hash_function);

/**
 * @brief Decode DER-encoded ECDSA signature.
 *
 * Format:
 *   SEQUENCE {
 *     INTEGER r
 *     INTEGER s
 *   }
 *
 * @throws std::runtime_error on invalid DER.
 */
ECDSASignature DER_decode_signature_ecdsa(const std::vector<uint8_t>& sig);

/**
 * @brief Decode DER signature from hex string.
 */
ECDSASignature DER_decode_signature_ecdsa(const std::string& hex_sig);

/**
 * @brief Encode ECDSA signature into DER format.
 */
std::vector<uint8_t> DER_encode_signature(const ECDSASignature& sig);

/**
 * @brief Encode ECDSA signature into hex-encoded DER.
 */
std::string DER_encode_signature_hex(const ECDSASignature& sig);

/**
 * @brief Encode public key into SEC1 format.
 *
 * @param key_pub Public key.
 * @param compressed Whether to use compressed format.
 * @return Byte vector containing encoded key.
 */
std::vector<uint8_t> encode_public_key(const ECDSAPublicKey& key_pub,
                                       bool compressed = false);

/**
 * @brief Encode public key into hex SEC1 format.
 */
std::string encode_public_key_hex(const ECDSAPublicKey& key_pub,
                                  bool compressed = false);

/**
 * @brief Decode SEC1-encoded public key.
 *
 * Supports both compressed and uncompressed formats.
 *
 * @throws std::runtime_error on invalid encoding.
 */
ECDSAPublicKey decode_public_key(const std::vector<uint8_t>& pub_key,
                                 bool compressed);

/**
 * @brief Decode hex SEC1-encoded public key.
 */
ECDSAPublicKey decode_public_key(const std::string& hex_pub_key,
                                 bool compressed);

}
