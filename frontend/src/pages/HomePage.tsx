import { useNavigate } from "react-router-dom";
import { Lock, ShieldCheck } from "lucide-react";

const HomePage = () => {
  const navigate = useNavigate();

  return (
    <div className="h-full flex flex-col items-center justify-center space-y-10 animate-in fade-in">
      <div className="text-center space-y-2">
        <h1 className="text-4xl font-bold text-[#0f172a]">Welcome to SecureSign</h1>
        <p className="text-slate-500 text-lg">What would you like to do today?</p>
      </div>

      <div className="grid grid-cols-1 md:grid-cols-2 gap-8 w-full max-w-4xl">
        <button
          onClick={() => navigate("/encrypt")}
          className="group flex flex-col items-center justify-center p-12 bg-white rounded-2xl shadow-sm border border-slate-100 hover:border-cyan-500 hover:shadow-md transition-all duration-200 hover:-translate-y-1"
        >
          <div className="w-20 h-20 bg-slate-50 group-hover:bg-cyan-50 rounded-full flex items-center justify-center mb-6 transition-colors">
            <Lock className="w-10 h-10 text-[#0f172a] group-hover:text-cyan-600 transition-colors" />
          </div>
          <h2 className="text-2xl font-bold text-[#0f172a] mb-2">Encrypt & Sign</h2>
          <p className="text-slate-500 text-center text-sm">
            Secure and sign your files digitally using RSA key.
          </p>
        </button>

        <button
          onClick={() => navigate("/verify")}
          className="group flex flex-col items-center justify-center p-12 bg-white rounded-2xl shadow-sm border border-slate-100 hover:border-cyan-500 hover:shadow-md transition-all duration-200 hover:-translate-y-1"
        >
          <div className="w-20 h-20 bg-slate-50 group-hover:bg-cyan-50 rounded-full flex items-center justify-center mb-6 transition-colors">
            <ShieldCheck className="w-10 h-10 text-[#0f172a] group-hover:text-cyan-600 transition-colors" />
          </div>
          <h2 className="text-2xl font-bold text-[#0f172a] mb-2">Verify Signature</h2>
          <p className="text-slate-500 text-center text-sm">
            Check the authenticity of the signature and integrity of the file from the ZIP package.
          </p>
        </button>
      </div>
    </div>
  );
};

export default HomePage;