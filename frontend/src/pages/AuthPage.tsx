import React, { useState, useEffect } from "react";
import { useNavigate } from "react-router-dom";
import { User, Lock, Shield, Key } from "lucide-react";
import { getCurrentWindow, LogicalSize } from "@tauri-apps/api/window";
import { verifyUserLogin } from "../services/serverAPI";

type ViewMode =
  | "LOGIN"
  | "LOGIN_PASSPHRASE"
  | "REGISTER_1"
  | "REGISTER_2"
  | "FORGOT_PASSWORD";

const AuthPage: React.FC = () => {
  const navigate = useNavigate();
  const [viewMode, setViewMode] = useState<ViewMode>("LOGIN");
  const [errorMessage, setErrorMessage] = useState<string>("");
  const [rememberMe, setRememberMe] = useState<boolean>(false);

  useEffect(() => {
    const shrinkWindow = async () => {
      try {
        const appWindow = getCurrentWindow();
        await appWindow.setResizable(false);
        await appWindow.setSize(new LogicalSize(420, 750));
        await appWindow.center();
      } catch (error) {
        console.error("Failed to shrink window:", error);
      }
    };

    if ((window as any).__TAURI_INTERNALS__) {
      shrinkWindow();
    }
  }, []);

  const [formData, setFormData] = useState({
    login: "",
    repeatLogin: "",
    password: "",
    repeatPassword: "",
    keyPassphrase: "",
    repeatKeyPassphrase: "",
  });

  const handleChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    setFormData({ ...formData, [e.target.name]: e.target.value });
    if (errorMessage) setErrorMessage("");
  };

  const changeView = (mode: ViewMode) => {
    setErrorMessage("");
    setViewMode(mode);
  };

  const finalizeAuth = () => {
    if (rememberMe) {
      localStorage.setItem("isAuthenticated", "true");
    } else {
      sessionStorage.setItem("isAuthenticated", "true");
      localStorage.removeItem("isAuthenticated");
    }
    navigate("/");
  };

  const handleSubmit = async (e: React.FormEvent<HTMLFormElement>) => {
    e.preventDefault();

    if (viewMode === "LOGIN") {
      const credentials = {
        login: formData.login,
        password_hash: formData.password,
      };

      const response = await verifyUserLogin(credentials);

      if (response) {
        changeView("LOGIN_PASSPHRASE");
      } else {
        setErrorMessage("Login failed: Invalid credentials.");
      }
      return;
    }

    if (viewMode === "LOGIN_PASSPHRASE") {
      // Placeholder for backend logic: verify private key passphrase
      const response = true;

      if (response) {
        finalizeAuth();
      } else {
        setErrorMessage("Invalid key passphrase.");
      }
      return;
    }

    if (viewMode === "REGISTER_1") {
      // Local validation (stays on the frontend)
      if (formData.login !== formData.repeatLogin) {
        setErrorMessage("Logins do not match!");
        return;
      }
      if (formData.password !== formData.repeatPassword) {
        setErrorMessage("Passwords do not match!");
        return;
      }

      // Placeholder for backend logic: check if user already exists
      const response = true;
      if (response) {
        changeView("REGISTER_2");
      } else {
        setErrorMessage("User with this login already exists.");
      }
      return;
    }

    if (viewMode === "REGISTER_2") {
      // Local validation
      if (formData.keyPassphrase !== formData.repeatKeyPassphrase) {
        setErrorMessage("Key passphrases do not match!");
        return;
      }

      // Placeholder for backend logic: send complete data to the database
      const response = true;
      if (response) {
        finalizeAuth();
      } else {
        setErrorMessage("Server error during account creation.");
      }
      return;
    }
  };

  return (
    <div className="min-h-screen bg-[#0f172a] flex items-center justify-center p-0 font-sans text-slate-900">
      <div className="w-full max-w-[420px] h-screen bg-white shadow-2xl flex flex-col overflow-hidden transition-all duration-300">
        <div className="bg-[#0f172a] py-10 px-6 flex flex-col items-center justify-center relative">
          <div className="relative">
            <div className="absolute inset-0 bg-cyan-500/20 blur-xl rounded-full"></div>
            <Shield className="w-14 h-14 text-cyan-400 relative z-10" />
          </div>
          <h1 className="text-3xl font-bold text-center mt-4 tracking-tight text-white">
            SecureSign
          </h1>
          <p className="text-cyan-400/60 text-xs mt-2 uppercase tracking-widest font-semibold">
            Identity Protection
          </p>
        </div>

        <div className="px-8 py-6 flex-1 flex flex-col overflow-y-auto">
          {viewMode === "FORGOT_PASSWORD" ? (
            <div className="text-center space-y-6 py-4 flex-1 flex flex-col justify-center">
              <h2 className="text-xl font-bold text-[#0f172a]">
                Work in progress
              </h2>
              <p className="text-slate-500 text-sm">
                This feature is not yet available.
              </p>
              <button
                onClick={() => changeView("LOGIN")}
                className="text-cyan-600 font-semibold hover:underline"
              >
                Back to login
              </button>
            </div>
          ) : (
            <form
              onSubmit={handleSubmit}
              className="space-y-5 flex flex-col justify-center mb-6"
            >
              {viewMode === "LOGIN" && (
                <>
                  <div className="relative">
                    <User className="absolute left-4 top-1/2 -translate-y-1/2 w-5 h-5 text-slate-400" />
                    <input
                      name="login"
                      type="text"
                      required
                      placeholder="Username"
                      className="w-full pl-12 pr-4 py-3.5 border border-slate-200 rounded-xl focus:ring-2 focus:ring-cyan-500/20 focus:border-cyan-500 outline-none transition-all placeholder:text-slate-400 bg-slate-50/50"
                      value={formData.login}
                      onChange={handleChange}
                    />
                  </div>
                  <div className="relative">
                    <Lock className="absolute left-4 top-1/2 -translate-y-1/2 w-5 h-5 text-slate-400" />
                    <input
                      name="password"
                      type="password"
                      required
                      placeholder="Password"
                      className="w-full pl-12 pr-4 py-3.5 border border-slate-200 rounded-xl focus:ring-2 focus:ring-cyan-500/20 focus:border-cyan-500 outline-none transition-all bg-slate-50/50"
                      value={formData.password}
                      onChange={handleChange}
                    />
                  </div>
                  <div className="flex items-center justify-between">
                    <label className="flex items-center space-x-2 cursor-pointer">
                      <input
                        type="checkbox"
                        checked={rememberMe}
                        onChange={(e) => setRememberMe(e.target.checked)}
                        className="w-4 h-4 rounded border-slate-300 text-[#1e40af] focus:ring-[#1e40af] bg-slate-50"
                      />
                      <span className="text-sm text-slate-500 font-medium">
                        Remember me
                      </span>
                    </label>
                    <button
                      type="button"
                      onClick={() => changeView("FORGOT_PASSWORD")}
                      className="text-sm font-medium text-slate-400 hover:text-[#0f172a] transition-colors underline underline-offset-4"
                    >
                      Forgot password?
                    </button>
                  </div>
                </>
              )}

              {viewMode === "LOGIN_PASSPHRASE" && (
                <>
                  <p className="text-sm text-slate-600 font-medium text-center mb-2">
                    Enter your private key passphrase to unlock your identity on
                    this device.
                  </p>
                  <div className="relative">
                    <Key className="absolute left-4 top-1/2 -translate-y-1/2 w-5 h-5 text-slate-400" />
                    <input
                      name="keyPassphrase"
                      type="password"
                      required
                      placeholder="Private Key Passphrase"
                      className="w-full pl-12 pr-4 py-3.5 border border-slate-200 rounded-xl focus:ring-2 focus:ring-cyan-500/20 focus:border-cyan-500 outline-none transition-all bg-slate-50/50"
                      value={formData.keyPassphrase}
                      onChange={handleChange}
                    />
                  </div>
                </>
              )}

              {viewMode === "REGISTER_1" && (
                <>
                  <div className="space-y-3">
                    <input
                      name="login"
                      type="text"
                      required
                      placeholder="New Username"
                      className="w-full px-4 py-3 border border-slate-200 rounded-xl outline-none bg-slate-50/50"
                      value={formData.login}
                      onChange={handleChange}
                    />
                    <input
                      name="repeatLogin"
                      type="text"
                      required
                      placeholder="Repeat Username"
                      className="w-full px-4 py-3 border border-slate-200 rounded-xl outline-none bg-slate-50/50"
                      value={formData.repeatLogin}
                      onChange={handleChange}
                    />
                    <input
                      name="password"
                      type="password"
                      required
                      placeholder="New Password"
                      className="w-full px-4 py-3 border border-slate-200 rounded-xl outline-none bg-slate-50/50"
                      value={formData.password}
                      onChange={handleChange}
                    />
                    <input
                      name="repeatPassword"
                      type="password"
                      required
                      placeholder="Repeat Password"
                      className="w-full px-4 py-3 border border-slate-200 rounded-xl outline-none bg-slate-50/50"
                      value={formData.repeatPassword}
                      onChange={handleChange}
                    />
                  </div>
                  <div className="mt-2 flex items-center justify-start">
                    <label className="flex items-center space-x-2 cursor-pointer">
                      <input
                        type="checkbox"
                        checked={rememberMe}
                        onChange={(e) => setRememberMe(e.target.checked)}
                        className="w-4 h-4 rounded border-slate-300 text-[#1e40af] focus:ring-[#1e40af] bg-slate-50"
                      />
                      <span className="text-sm text-slate-500 font-medium">
                        Remember me
                      </span>
                    </label>
                  </div>
                </>
              )}

              {viewMode === "REGISTER_2" && (
                <>
                  <p className="text-sm text-slate-600 font-medium text-center mb-4">
                    Set a passphrase to encrypt your private key locally. This
                    cannot be recovered.
                  </p>
                  <div className="space-y-3">
                    <input
                      name="keyPassphrase"
                      type="password"
                      required
                      placeholder="Key Passphrase"
                      className="w-full px-4 py-3 border border-slate-200 rounded-xl outline-none bg-slate-50/50"
                      value={formData.keyPassphrase}
                      onChange={handleChange}
                    />
                    <input
                      name="repeatKeyPassphrase"
                      type="password"
                      required
                      placeholder="Repeat Passphrase"
                      className="w-full px-4 py-3 border border-slate-200 rounded-xl outline-none bg-slate-50/50"
                      value={formData.repeatKeyPassphrase}
                      onChange={handleChange}
                    />
                  </div>
                </>
              )}

              {errorMessage && (
                <p className="text-red-500 text-sm font-bold text-center animate-bounce">
                  {errorMessage}
                </p>
              )}

              <button
                type="submit"
                className="w-full bg-[#1e40af] hover:bg-[#1e3a8a] text-white py-4 rounded-xl font-bold shadow-lg active:scale-95 transition-all mt-4 text-sm tracking-widest"
              >
                {viewMode === "LOGIN" && "SIGN IN"}
                {viewMode === "LOGIN_PASSPHRASE" && "CONFIRM"}
                {viewMode === "REGISTER_1" && "CONTINUE"}
                {viewMode === "REGISTER_2" && "FINISH SETUP"}
              </button>
            </form>
          )}

          {viewMode !== "FORGOT_PASSWORD" && (
            <div className="mt-auto pt-6 border-t border-slate-100 text-center">
              {viewMode === "LOGIN" && (
                <p className="text-sm text-slate-500">
                  New here?
                  <button
                    type="button"
                    onClick={() => changeView("REGISTER_1")}
                    className="ml-2 font-bold text-[#0f172a] hover:text-cyan-600 transition-colors"
                  >
                    Create Account
                  </button>
                </p>
              )}

              {viewMode === "LOGIN_PASSPHRASE" && (
                <button
                  type="button"
                  onClick={() => changeView("FORGOT_PASSWORD")}
                  className="text-sm font-bold text-slate-400 hover:text-red-500 transition-colors"
                >
                  Reset private key
                </button>
              )}

              {(viewMode === "REGISTER_1" || viewMode === "REGISTER_2") && (
                <button
                  type="button"
                  onClick={() => changeView("LOGIN")}
                  className="text-sm font-bold text-[#0f172a] hover:text-cyan-600 transition-colors"
                >
                  Back to Login
                </button>
              )}
            </div>
          )}
        </div>
      </div>
    </div>
  );
};

export default AuthPage;
