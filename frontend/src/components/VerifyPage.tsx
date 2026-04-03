import { FileText, Check } from "lucide-react";

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
      </div>

      <div className="bg-white p-12 rounded-2xl shadow-sm border border-slate-100 flex flex-col items-center justify-center">
        <div className="flex items-center space-x-3">
          <label>
            <input
              type="text"
              name=""
              id=""
              placeholder="Enter public key"
              className="
    w-full
    px-8 py-3
    rounded-xl
    border border-slate-200
    shadow-md
    text-[#0f172a]
    placeholder-slate-400
    focus:outline-none
    focus:ring-2 focus:ring-[#1e40af]
    focus:border-[#1e40af]
    transition-all
  "
            />
          </label>
          <label>
            <input
              type="text"
              name=""
              id=""
              placeholder="Enter signature"
              className="
    w-full
    px-8 py-3
    rounded-xl
    border border-slate-200
    shadow-md
    text-[#0f172a]
    placeholder-slate-400
    focus:outline-none
    focus:ring-2 focus:ring-[#1e40af]
    focus:border-[#1e40af]
    transition-all
  "
            />
          </label>
        </div>

        <button
          type="button"
          className="bg-[#1e40af] hover:bg-[#1e3a8a] text-white px-8 py-3 rounded-xl font-semibold shadow-md transition-colors cursor-pointer mt-3"
        >
          Verify signature
        </button>
      </div>

      <div className="bg-white p-12 rounded-2xl shadow-sm border border-slate-100 flex flex-col items-center justify-center">
        <span className="text-[#0f172a] font-medium">
          Signed by: Jan Kowalski (jan.kowalski@firma.pl)
        </span>
        <span className="text-[#0f172a] font-medium">
          ID certificate: A1B2-C3D4-E5F6
        </span>
        <span className="text-[#0f172a] font-medium">
          Date of signature: 02.04.2026, 11:32 AM
        </span>
      </div>
    </div>
  );
};

export default VerifyPage;
