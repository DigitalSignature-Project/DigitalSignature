import { FileText, Check, ShieldAlert } from "lucide-react";
import { VerifySignatureBtn } from "../components/VerifySignatureBtn";
import { calculateRsa } from "../services/rsaAPI";

const GradientCheck = () => (
  <svg className="w-40 h-40" viewBox="0 0 24 24" fill="none" strokeWidth={2}>
    <defs>
      <linearGradient id="greenGradient" x1="0%" y1="0%" x2="10%" y2="100%">
        <stop offset="0%" stopColor="#22c55e" />
        <stop offset="100%" stopColor="#166534" />
      </linearGradient>
    </defs>
    <Check stroke="url(#greenGradient)" />
  </svg>
);

const VerifyPage = () => {
  return (
    <div className="max-w-4xl mx-auto space-y-8">
      <p className="font-bold text-[#0f172a] text-4xl text-center mt-2">
        Digital signature verification
      </p>

      <div className="flex items-center space-x-6 justify-center">
        <FileText className="w-20 h-20 text-[#0f172a] mt-2" />

        <div className="flex flex-col">
          <span className="text-[#0f172a] text-2xl font-medium mt-2">
            Document_title.pdf
          </span>
          <span className="text-[#0f172a] text-2xl">(3.4MB)</span>
        </div>
      </div>

      <div className="bg-white p-12 rounded-2xl shadow-sm border border-slate-100 flex flex-col items-center justify-center">
        <div className="flex items-center space-x-6 justifi-center">
          <GradientCheck />

          <div className="flex flex-col">
            <span className="text-[#0f172a] font-bold text-4xl">
              Signature correct
            </span>
            <span className="text-[#0f172a] text-2xl text-center mt-3">
              File verified successfully
            </span>
          </div>
        </div>

        <div className="w-120 bg-slate-200 rounded-full h-0.5 mt-6"></div>

        <VerifySignatureBtn
          onClick={async () => {
            const bits = 1024;
            const data = await calculateRsa(bits);
            console.log("Wygenerowane klucze (guzik verify signature):", data);
          }}
        />
      </div>

      <div
        className="bg-white p-12 rounded-2xl shadow-sm border border-slate-100 
                flex flex-col justify-center 
                w-full mx-auto text-left"
      >
        <div className="max-w-xl mx-auto text-left flex flex-col">
          <span className="text-[#0f172a] font-medium">
            Signed by: Jan Kowalski (jan.kowalski@firma.pl)
          </span>
          <span className="text-[#0f172a] font-medium mt-1">
            ID certificate: A1B2-C3D4-E5F6
          </span>
          <span className="text-[#0f172a] font-medium mt-1">
            Date of signature: 02.04.2026, 11:32 AM
          </span>
        </div>
      </div>

      <div className="flex items-center space-x-6 justify-center">
        <ShieldAlert className="w-20 h-20 text-[#0f172a] mt-2" />

        <div>
          <span className="text-[#0f172a] font-bold">Warning: </span>
          <span className="text-[#0f172a] font-medium">
            file has not been changed since it was signed
          </span>
        </div>
      </div>
    </div>
  );
};

export default VerifyPage;
