import { Plus, FileText, Lock } from "lucide-react";
import { EncryptAndSignBtn } from "../components/EncryptAndSignBtn";
import { calculateRsa } from "../services/rsaAPI";

const EncryptPage = () => {
  return (
    <div className="max-w-4xl mx-auto space-y-8">
      <div className="bg-white p-12 rounded-2xl shadow-sm border border-slate-100 flex flex-col items-center justify-center">
        <div className="w-64 h-64 rounded-full border-2 border-dashed border-slate-300 flex flex-col items-center justify-center cursor-pointer hover:border-cyan-500 hover:bg-cyan-50/50 transition-all group">
          <Plus className="w-16 h-16 text-[#0f172a] mb-2 group-hover:scale-110 transition-transform" />
          <p className="text-center text-sm text-slate-500">
            Drag and drop file here or
            <br />
            <span className="font-bold text-[#0f172a]">CLICK TO CHOOSE</span>
          </p>
        </div>

        <div className="mt-12 w-full max-w-md">
          <div className="flex items-center space-x-4">
            <FileText className="w-10 h-10 text-[#0f172a]" />
            <div className="flex-1">
              <div className="flex justify-between text-sm mb-1">
                <span className="font-medium text-slate-700">
                  Trade_Agreement.pdf (3.4 MB)
                </span>
                <Lock className="w-4 h-4 text-[#0f172a]" />
              </div>
              <div className="w-full bg-slate-200 rounded-full h-2">
                <div className="bg-[#0f172a] h-2 rounded-full w-[80%]"></div>
              </div>
            </div>
          </div>
        </div>

        <div className="mt-12 w-full max-w-md flex items-end justify-between border-t border-slate-100 pt-8">
          <div className="space-y-3">
            <label className="flex items-center space-x-3 cursor-pointer">
              <input
                type="checkbox"
                defaultChecked
                className="w-5 h-5 rounded border-slate-300 text-[#0f172a] focus:ring-[#0f172a]"
              />
              <span className="text-slate-700 font-medium">
                AES-256 encryption
              </span>
            </label>
            <label className="flex items-center space-x-3 cursor-pointer">
              <input
                type="checkbox"
                defaultChecked
                className="w-5 h-5 rounded border-slate-300 text-[#0f172a] focus:ring-[#0f172a]"
              />
              <span className="text-slate-700 font-medium">
                Digital Signature (Your Identity)
              </span>
            </label>
          </div>

          <EncryptAndSignBtn
            onClick={async () => {
              const bits = 1024;
              const data = await calculateRsa(bits);
              console.log(
                "Wygenerowane klucze (guzik encrypt and sign):",
                data,
              );
            }}
          />
        </div>
      </div>
    </div>
  );
};

export default EncryptPage;
