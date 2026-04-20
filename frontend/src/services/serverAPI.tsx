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

type RegiserNewUser = {
  login: string;
  password_hash: string;
  public_key: string;
  encrypted_private_key: string;
};

type RegisterNewUserResponse = {
  status_code: string;
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

  if (data.message === "Login successful") {
    return { success: true, data };
  }

  return { success: false, message: data.message };
};

export const registerNewUser = async (credentials: RegiserNewUser) => {
  const { data } = await axios.post<RegisterNewUserResponse>(
    "http://127.0.0.1:2138/server/register_new_user",
    {
      login: credentials.login,
      password_hash: credentials.password_hash,
      public_key: credentials.public_key,
      encrypted_private_key: credentials.encrypted_private_key,
    },
    {
      headers: {
        Authorization: "Bearer 2137",
      },
    },
  );

  return Number(data.status_code) === 201 ? true : false;
};
