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
  result: boolean;
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
) => {
  const { data } = await axios.post<SignInFileRsaResponse>(
    "http://127.0.0.1:2138/signature/generate_rsa_signature",
    { file_content, login, password },
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
) => {
  const { data } = await axios.post<VerifyRsaSignatureResponse>(
    "http://127.0.0.1:2138/signature/verify_rsa_signature",
    { file_content, signature, login },
    {
      headers: {
        Authorization: "Bearer 2137",
      },
    },
  );
  return data;
};
