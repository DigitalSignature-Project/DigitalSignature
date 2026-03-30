type RsaResponse = {
  key_pub: string;
  key_priv: string;
  key_module: string;
};

export const calculateRsa = async (bits: number): Promise<RsaResponse> => {
  const response = await fetch("http://127.0.0.1:8000/api/rsa_generate_keys", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({
      bits: Number(bits),
    }),
  });

  const data: { key_pub: string; key_priv: string; key_module: string } =
    await response.json();
  return data;
};

export const calculateRsaParallel = async (
  bits: number,
  threads: number,
): Promise<RsaResponse> => {
  const response = await fetch(
    "http://127.0.0.1:8000/api/rsa_generate_keys_parallel",
    {
      method: "POST",
      headers: { "Content-Type": "application/json" },
      body: JSON.stringify({
        bits: Number(bits),
        threads: Number(threads),
      }),
    },
  );

  const data: { key_pub: string; key_priv: string; key_module: string } =
    await response.json();
  return data;
};
