import axios from "axios";

type VerifyUserLogin = {
  login: string;
  password_hash: string;
};

type VerifyUserLoginResponse = {
  message: string;
  encrypted_private_key: string;
  public_key: string;
};

export const verifyUserLogin = async (credentials: VerifyUserLogin) => {
  const { data } = await axios.post<VerifyUserLoginResponse>(
    "http://127.0.0.1:2138/server/verify_user_login",
    {
      login: credentials.login,
      password_hash: credentials.password_hash,
    },
    {
      headers: {
        Authorization: "Bearer 2137",
      },
    },
  );

  return data.message === "Login successful" ? true : false;
};
