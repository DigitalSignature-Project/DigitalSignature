import axios from "axios";

type RsaResponse = {
  key_pub: string;
  key_priv: string;
  key_module: string;
};

type SignInFileRsaResponse = {
  signature: string;
};

type VerifyRsaSignatureResponse = {
  is_valid: boolean;
};

export const calculateRsa = async (bits: number) => {
  const { data } = await axios.post<RsaResponse>(
    "http://127.0.0.1:2138/api/rsa_generate_keys",
    { bits },
    {
      headers: {
        Authorization: "Bearer 2137",
      },
    },
  );
  return data;
};

export const calculateRsaParallel = async (bits: number, threads: number) => {
  const { data } = await axios.post<RsaResponse>(
    "http://127.0.0.1:2138/api/rsa_generate_keys_parallel",
    { bits, threads },
    {
      headers: {
        Authorization: "Bearer 2137",
      },
    },
  );
  return data;
};

export const signInRsaFile = async (
  file_content: string,
  login: string,
  password: string,
  encrypted_private_key: string,
  key_module: string,
  salt_length: string,
  hash_function_1: string,
  hash_function_2: string,
) => {
  const { data } = await axios.post<SignInFileRsaResponse>(
    "http://127.0.0.1:2138/signature/generate_rsa_signature",
    {
      file_content,
      login,
      password,
      encrypted_private_key,
      key_module,
      salt_length,
      hash_function_1,
      hash_function_2,
    },
    {
      headers: {
        Authorization: "Bearer 2137",
      },
    },
  );

  return data;
};

export const signInElgamalFile = async (
  file_content: string,
  login: string,
  password: string,
  encrypted_private_key: string,
  key_module: string,
  hash: string,
) => {
  const { data } = await axios.post<SignInFileRsaResponse>(
    "http://127.0.0.1:2138/signature/generate_elgamal_signature",
    {
      file_content,
      login,
      password,
      encrypted_private_key,
      key_module,
      hash,
    },
    {
      headers: {
        Authorization: "Bearer 2137",
      },
    },
  );

  return data;
};

export const signInEcdsaFile = async (
  file_content: string,
  login: string,
  password: string,
  encrypted_private_key: string,
  key_module: string,
  hash: string,
) => {
  const { data } = await axios.post<SignInFileRsaResponse>(
    "http://127.0.0.1:2138/signature/generate_ecdsa_signature",
    {
      file_content,
      login,
      password,
      encrypted_private_key,
      key_module,
      hash,
    },
    {
      headers: {
        Authorization: "Bearer 2137",
      },
    },
  );

  return data;
};

export const verifyRsaSignature = async (
  file_content: string,
  signature: string,
  login: string,
  salt_length: string,
  hash_function_1: string,
  hash_function_2: string,
) => {
  const { data } = await axios.post<VerifyRsaSignatureResponse>(
    "http://127.0.0.1:2138/signature/verify_rsa_signature",
    {
      file_content,
      signature,
      login,
      salt_length,
      hash_function_1,
      hash_function_2,
    },
    {
      headers: {
        Authorization: "Bearer 2137",
      },
    },
  );

  return data;
};

export const verifyElgamalSignature = async (
  file_content: string,
  signature: string,
  login: string,
  hash: string,
) => {
  const { data } = await axios.post<VerifyRsaSignatureResponse>(
    "http://127.0.0.1:2138/signature/verify_elgamal_signature",
    {
      file_content,
      signature,
      login,
      hash,
    },
    {
      headers: {
        Authorization: "Bearer 2137",
      },
    },
  );

  return data;
};

export const verifyEcdsaSignature = async (
  file_content: string,
  signature: string,
  login: string,
  hash: string,
) => {
  const { data } = await axios.post<VerifyRsaSignatureResponse>(
    "http://127.0.0.1:2138/signature/verify_ecdsa_signature",
    {
      file_content,
      signature,
      login,
      hash,
    },
    {
      headers: {
        Authorization: "Bearer 2137",
      },
    },
  );

  return data;
};
