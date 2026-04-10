import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { User, Lock, Mail, Fingerprint, ScanFace, Shield } from 'lucide-react'; 

const AuthPage: React.FC = () => {
  const navigate = useNavigate();
  const [viewMode, setViewMode] = useState<number>(0); 
  const [formData, setFormData] = useState({ username: '', email: '', password: '', confirmPassword: '' });
  const [errorMessage, setErrorMessage] = useState<string>('');

  const handleChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    setFormData({ ...formData, [e.target.name]: e.target.value });
    if (errorMessage) setErrorMessage('');
  };

  const setView = (mode: number) => {
    setFormData({ username: '', email: '', password: '', confirmPassword: '' });
    setErrorMessage('');
    setViewMode(mode);
  };

  const handleSubmit = (e: React.FormEvent) => {
    e.preventDefault();
    if (viewMode === 1 && formData.password !== formData.confirmPassword) {
      setErrorMessage('Hasła nie są identyczne!');
      return;
    }
    navigate('/'); 
  };

  return (
    <div className="min-h-screen bg-slate-50 flex items-center justify-center p-4 font-sans text-slate-900">
      <div className="w-full max-w-[420px] bg-white rounded-2xl shadow-sm border border-slate-100 overflow-hidden transition-all">
        
        <div className="bg-[#0f172a] py-12 flex flex-col items-center justify-center">
          <div className="relative">
            <div className="absolute inset-0 bg-cyan-500/20 blur-xl rounded-full"></div>
            <Shield className="w-16 h-16 text-cyan-400 relative z-10" />
          </div>
        </div>

        <div className="p-10"> 
          <h1 className="text-3xl font-bold text-center mb-10 tracking-tight text-[#0f172a]">
            SecureSign
          </h1>

          <form onSubmit={handleSubmit} className="space-y-5">
            {viewMode !== 2 && (
              <>
                <div className="relative">
                  <User className="absolute left-4 top-1/2 -translate-y-1/2 w-5 h-5 text-slate-400" />
                  <input
                    name="username" type="text" required
                    placeholder={viewMode === 0 ? "Username / Email" : "New username"}
                    className="w-full pl-12 pr-4 py-3.5 border border-slate-200 rounded-xl focus:ring-2 focus:ring-cyan-500/20 focus:border-cyan-500 outline-none transition-all placeholder:text-slate-400 bg-slate-50/50"
                    value={formData.username} onChange={handleChange}
                  />
                </div>

                {viewMode === 1 && (
                  <div className="relative">
                    <Mail className="absolute left-4 top-1/2 -translate-y-1/2 w-5 h-5 text-slate-400" />
                    <input
                      name="email" type="email" required placeholder="Email address"
                      className="w-full pl-12 pr-4 py-3.5 border border-slate-200 rounded-xl focus:ring-2 focus:ring-cyan-500/20 focus:border-cyan-500 outline-none transition-all bg-slate-50/50"
                      value={formData.email} onChange={handleChange}
                    />
                  </div>
                )}

                <div className="relative">
                  <Lock className="absolute left-4 top-1/2 -translate-y-1/2 w-5 h-5 text-slate-400" />
                  <input
                    name="password" type="password" required
                    placeholder={viewMode === 0 ? "Password" : "Create password"}
                    className="w-full pl-12 pr-4 py-3.5 border border-slate-200 rounded-xl focus:ring-2 focus:ring-cyan-500/20 focus:border-cyan-500 outline-none transition-all bg-slate-50/50"
                    value={formData.password} onChange={handleChange}
                  />
                </div>

                {viewMode === 1 && (
                  <div className="relative">
                    <Lock className="absolute left-4 top-1/2 -translate-y-1/2 w-5 h-5 text-slate-400" />
                    <input
                      name="confirmPassword" type="password" required placeholder="Confirm password"
                      className={`w-full pl-12 pr-4 py-3.5 border rounded-xl focus:ring-2 outline-none transition-all bg-slate-50/50 ${errorMessage ? 'border-red-500 focus:ring-red-100' : 'focus:ring-cyan-500/20 focus:border-cyan-500 border-slate-200'}`}
                      value={formData.confirmPassword} onChange={handleChange}
                    />
                  </div>
                )}

                {errorMessage && (
                  <p className="text-red-500 text-xs font-bold text-center mt-1">
                    {errorMessage}
                  </p>
                )}

                {viewMode === 0 && (
                  <div className="text-right">
                    <button type="button" onClick={() => setView(2)} className="text-sm font-medium text-slate-400 hover:text-[#0f172a] transition-colors underline underline-offset-4">
                      Forgot password?
                    </button>
                  </div>
                )}
              </>
            )}

            {viewMode === 2 && (
              <div className="space-y-4">
                <p className="text-center text-slate-500 text-sm">Enter email to reset password</p>
                <div className="relative">
                  <Mail className="absolute left-4 top-1/2 -translate-y-1/2 w-5 h-5 text-slate-400" />
                  <input
                    name="email" type="email" required placeholder="Email"
                    className="w-full pl-12 pr-4 py-3.5 border border-slate-200 rounded-xl focus:ring-2 focus:ring-cyan-500/20 outline-none bg-slate-50/50"
                    value={formData.email} onChange={handleChange}
                  />
                </div>
              </div>
            )}

            <button
              type="submit"
              className="w-full bg-[#1e40af] hover:bg-[#1e3a8a] text-white py-4 rounded-xl font-semibold shadow-md active:scale-95 transition-all mt-4 text-sm tracking-wide"
            >
              {viewMode === 0 ? 'SIGN IN' : viewMode === 1 ? 'CREATE ACCOUNT' : 'SEND RESET LINK'}
            </button>
          </form>

          <div className="mt-10 pt-8 border-t border-slate-100 text-center">
            {viewMode === 0 && (
              <div className="flex items-center justify-center gap-4 mb-8 text-slate-400 hover:text-[#0f172a] cursor-pointer transition-all group">
                <Fingerprint className="w-6 h-6 group-hover:text-cyan-600" />
                <ScanFace className="w-6 h-6 group-hover:text-cyan-600" />
                <span className="text-[10px] font-bold tracking-[0.2em] uppercase">lub użyj biometrii</span>
              </div>
            )}

            <p className="text-sm text-slate-500">
              {viewMode === 0 && "Don't have an account?"}
              {viewMode === 1 && "Already have an account?"}
              {viewMode === 2 && "Remembered your password?"}
              <button
                type="button"
                onClick={() => {
                  if (viewMode === 2) setView(0); 
                  else setView(viewMode === 1 ? 0 : 1); 
                }}
                className="ml-2 font-bold text-[#0f172a] hover:text-cyan-600 transition-colors"
              >
                {viewMode === 0 && "Register"}
                {viewMode === 1 && "Login"}
                {viewMode === 2 && "Login"} 
              </button>
            </p>
          </div>
        </div>
      </div>
    </div>
  );
};

export default AuthPage;