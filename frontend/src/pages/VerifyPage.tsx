import { FileText, Check, ShieldAlert, Plus } from "lucide-react";
import { VerifySignatureBtn } from "../components/VerifySignatureBtn";
import { useState, useRef } from "react";
import {
  verifyRsaSignature,
  verifyElgamalSignature,
  verifyEcdsaSignature,
} from "../services/rsaAPI";
import JSZip from "jszip";

const GradientCheck = () => (
  <svg
    className="w-12 h-12 mr-4"
    viewBox="0 0 24 24"
    fill="none"
    strokeWidth={3}
  >
    <defs>
      <linearGradient id="greenGradient" x1="0%" y1="0%" x2="100%" y2="100%">
        <stop offset="0%" stopColor="#22c55e" />
        <stop offset="100%" stopColor="#16a34a" />
      </linearGradient>
    </defs>
    <Check stroke="url(#greenGradient)" />
  </svg>
);

type VerifyFileResponse = {
  isValid: boolean;
  signer: string;
  date: string;
};

const VerifyPage = () => {
  const [loading, setLoading] = useState<boolean>(false);
  const [selectedFile, setSelectedFile] = useState<File | null>(null);
  const [result, setResult] = useState<VerifyFileResponse | null>(null);
  const [progress, setProgress] = useState<number>(0);
  const fileInputRef = useRef<HTMLInputElement>(null);

  const handleVerifySignature = async () => {
    if (!selectedFile) return;

    setLoading(true);
    setResult(null);
    setProgress(0);

    const progressInterval = setInterval(() => {
      setProgress((p) => {
        if (p < 50) return p + 6;
        if (p < 80) return p + 3;
        if (p < 90) return p + 1;
        return 90;
      });
    }, 150);

    try {
      const zip = new JSZip();
      const zipContents = await zip.loadAsync(selectedFile);

      const signatureInfoFile = zipContents.file("signature_info.json");
      if (!signatureInfoFile) {
        throw new Error("signature_info.json not found in archive");
      }

      const signatureInfoContent = await signatureInfoFile.async("string");
      const signatureInfo = JSON.parse(signatureInfoContent);

      const originalFileName = signatureInfo.originalFileName;
      const originalFile = zipContents.file(originalFileName);
      if (!originalFile) {
        throw new Error(
          `Original file ${originalFileName} not found in archive`,
        );
      }

      const originalFileContent = await originalFile.async("uint8array");
      const file_content = Array.from(originalFileContent)
        .map((b) => b.toString(16).padStart(2, "0"))
        .join("");

      const signature = signatureInfo.signatureDetails.signature;
      const login = signatureInfo.user;
      const algorithm = signatureInfo.algorithm;
      const options = signatureInfo.algorithmOptions;

      let isValid = false;

      if (algorithm === "RSA") {
        const result = await verifyRsaSignature(
          file_content,
          signature,
          login,
          options.saltLength.toString(),
          options.hash_function_1,
          options.hash_function_2,
        );
        isValid = result.is_valid;
      } else if (algorithm === "ElGamal") {
        const result = await verifyElgamalSignature(
          file_content,
          signature,
          login,
          options.hash_function_1,
        );
        isValid = result.is_valid;
      } else if (algorithm === "ECDSA") {
        const result = await verifyEcdsaSignature(
          file_content,
          signature,
          login,
          options.hash_function_1,
        );
        isValid = result.is_valid;
      } else {
        throw new Error(`Unknown algorithm: ${algorithm}`);
      }

      clearInterval(progressInterval);
      setProgress(100);

      setResult({
        isValid,
        signer: login,
        date: signatureInfo.timestamp,
      });
    } catch (error) {
      console.error("Błąd podczas weryfikacji:", error);
      clearInterval(progressInterval);
      setProgress(0);
      setResult({
        isValid: false,
        signer: "Brak danych (Błąd weryfikacji)",
        date: "Brak danych",
      });
    } finally {
      setLoading(false);
    }
  };

  const handleFileChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    if (e.target.files && e.target.files.length > 0) {
      setSelectedFile(e.target.files[0]);
      setResult(null);
      setProgress(0);
    }
  };

  const handleDrop = (e: React.DragEvent<HTMLDivElement>) => {
    e.preventDefault();
    if (e.dataTransfer.files && e.dataTransfer.files.length > 0) {
      setSelectedFile(e.dataTransfer.files[0]);
      setResult(null);
      setProgress(0);
    }
  };

  const handleDragOver = (e: React.DragEvent<HTMLDivElement>) => {
    e.preventDefault();
  };

  const formatBytes = (bytes: number) =>
    (bytes / (1024 * 1024)).toFixed(2) + " MB";

  return (
    <div className="max-w-4xl mx-auto space-y-8">
      <p className="font-bold text-[#0f172a] text-4xl text-center mt-2">
        Digital signature verification
      </p>

      <div className="bg-white p-12 rounded-2xl shadow-sm border border-slate-100 flex flex-col items-center justify-center mt-8 min-h-[400px]">
        <input
          type="file"
          ref={fileInputRef}
          onChange={handleFileChange}
          accept=".ss,.zip"
          className="hidden"
        />

        {!selectedFile ? (
          <div
            onClick={() => fileInputRef.current?.click()}
            onDrop={handleDrop}
            onDragOver={handleDragOver}
            className="w-64 h-64 rounded-full border-2 border-dashed border-slate-300 flex flex-col items-center justify-center cursor-pointer hover:border-cyan-500 hover:bg-cyan-50/50 transition-all group animate-in fade-in"
          >
            <Plus className="w-16 h-16 text-[#0f172a] mb-2 group-hover:scale-110 transition-transform" />
            <p className="text-center text-sm text-slate-500">
              Click to choose
              <br />
              <span className="font-bold text-[#0f172a]">FILE</span>
            </p>
          </div>
        ) : (
          <div className="flex flex-col items-center mb-8 w-full max-w-md animate-in fade-in slide-in-from-bottom-4">
            <div
              className="flex items-center space-x-4 cursor-pointer hover:opacity-80 transition-opacity w-full"
              onClick={() => fileInputRef.current?.click()}
            >
              <FileText className="w-12 h-12 text-[#0f172a]" />
              <div className="flex-1 overflow-hidden">
                <div className="flex justify-between items-baseline text-sm mb-1">
                  <span className="text-[#0f172a] font-medium text-lg truncate pr-2">
                    {selectedFile.name}
                  </span>
                  <span className="text-slate-500 whitespace-nowrap">
                    {formatBytes(selectedFile.size)}
                  </span>
                </div>
                <span className="text-xs text-cyan-600 font-semibold block">
                  Click to change file
                </span>
              </div>
            </div>

            <div className="w-full bg-slate-200 rounded-full h-2 overflow-hidden mt-6">
              <div
                className="bg-[#0f172a] h-2 rounded-full transition-[width] duration-150 ease-linear"
                style={{ width: `${progress}%` }}
              ></div>
            </div>
          </div>
        )}

        {selectedFile && (
          <div className="flex flex-col items-center w-full max-w-md animate-in fade-in slide-in-from-bottom-2">
            <div className="w-full bg-slate-200 rounded-full h-px mb-8"></div>
            <VerifySignatureBtn
              onClick={handleVerifySignature}
              disabled={loading}
            />
          </div>
        )}
      </div>

      {result && (
        <div className="animate-in fade-in slide-in-from-bottom-4 space-y-6">
          {result.signer !== "Brak danych (Błąd API)" && (
            <div className="bg-white p-8 rounded-2xl shadow-sm border border-slate-100 flex flex-col w-full mx-auto text-left">
              <div className="text-left flex flex-col space-y-2">
                <span className="text-[#0f172a] font-medium text-lg">
                  Signed by: <span className="font-bold">{result.signer}</span>
                </span>
                <span className="text-[#0f172a] font-medium text-lg">
                  Date of signature:{" "}
                  <span className="font-bold">{result.date}</span>
                </span>
              </div>
            </div>
          )}

          <div
            className={`p-6 rounded-2xl shadow-sm border flex items-center justify-center ${result.isValid ? "bg-green-50 border-green-200" : "bg-red-50 border-red-200"}`}
          >
            {result.isValid ? (
              <div className="flex items-center">
                <GradientCheck />
                <span className="text-green-800 font-bold text-xl">
                  File has not been changed since it was signed
                </span>
              </div>
            ) : (
              <div className="flex items-center">
                <ShieldAlert className="w-12 h-12 text-red-600 mr-4" />
                <div className="flex flex-col">
                  <span className="text-red-600 font-bold text-xl">
                    Warning:
                  </span>
                  <span className="text-red-800 font-medium text-lg">
                    File has been changed since it was signed!
                  </span>
                </div>
              </div>
            )}
          </div>
        </div>
      )}
    </div>
  );
};

export default VerifyPage;
