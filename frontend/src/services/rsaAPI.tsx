import axios from "axios";

type RsaResponse = {
  key_pub: string;
  key_priv: string;
  key_module: string;
};

export const calculateRsa = async (bits: number) => {
  const { data } = await axios.post<RsaResponse>(
    "http://127.0.0.1:8000/api/rsa_generate_keys",
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
    "http://127.0.0.1:8000/api/rsa_generate_keys_parallel",
    { bits, threads },
    {
      headers: {
        Authorization: "Bearer 2137",
      },
    },
  );
  return data;
};
