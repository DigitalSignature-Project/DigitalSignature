import { Plus, FileText, Download } from "lucide-react";
import { EncryptAndSignBtn } from "../components/EncryptAndSignBtn";
import { TempResultSection } from "../components/TempResultSection";
import { useState, useRef } from "react";
import { useLocation } from "react-router-dom";
import { invoke } from "@tauri-apps/api/core";
import {
  signInRsaFile,
  signInElgamalFile,
  signInEcdsaFile,
} from "../services/rsaAPI";
import JSZip from "jszip";
import { save } from "@tauri-apps/plugin-dialog";
import { writeFile } from "@tauri-apps/plugin-fs";

const EncryptPage = () => {
  const location = useLocation();
  const [loading, setLoading] = useState<boolean>(false);
  const [calculated, setCalculated] = useState<string>("");

  const [selectedFile, setSelectedFile] = useState<File | null>(null);
  const [progress, setProgress] = useState<number>(0);
  const [algorithm, setAlgorithm] = useState<string>("RSA");
  const [saltLength, setSaltLength] = useState<number>(32);
  const [hash_function_1, setHash_function_1] = useState<string>("SHA256");
  const [hash_function_2, setHash_function_2] = useState<string>("SHA256");
  const fileInputRef = useRef<HTMLInputElement>(null);

  const [isSigned, setIsSigned] = useState<boolean>(false);
  const [signatureData, setSignatureData] = useState<any>(null);

  const login =
    localStorage.getItem("login") || sessionStorage.getItem("login");

  const getDecryptionKey = async (): Promise<string | null> => {
    const fromState = (location.state as { keyPassphrase?: string } | null)
      ?.keyPassphrase;
    if (fromState) return fromState;

    const fromSession = sessionStorage.getItem("keyPassphrase");
    if (fromSession) return fromSession;

    if (!login) return null;

    try {
      return (await invoke("get_credentials", { login })) as string;
    } catch (error) {
      console.error("No password saved or access error", error);
      return null;
    }
  };

  const handleEncryptAndSign = async () => {
    if (!selectedFile) return;

    const buffer = await selectedFile.arrayBuffer();
    const bytes = new Uint8Array(buffer);

    const file_content = Array.from(bytes)
      .map((b) => b.toString(16).padStart(2, "0"))
      .join("");

    if (!login) {
      setCalculated("Error: User is not logged in.");
      return;
    }

    const password = await getDecryptionKey();
    if (!password) {
      setCalculated("Error: No password for key. Log in again.");
      return;
    }

    const encryptedPrivateKey = sessionStorage.getItem("encryptedPrivateKey");
    const keyModule = sessionStorage.getItem("keyModule");

    if (!encryptedPrivateKey || !keyModule) {
      setCalculated("Error: No session keys. Log in again.");
      return;
    }

    setLoading(true);
    setIsSigned(false);
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
      let signed_file;
      if (algorithm === "RSA") {
        signed_file = await signInRsaFile(
          file_content,
          login,
          password,
          encryptedPrivateKey,
          keyModule,
          saltLength.toString(),
          hash_function_1,
          hash_function_2,
        );
      } else if (algorithm === "ElGamal") {
        signed_file = await signInElgamalFile(
          file_content,
          login,
          password,
          encryptedPrivateKey,
          keyModule,
          hash_function_1,
        );
      } else if (algorithm === "ECDSA") {
        signed_file = await signInEcdsaFile(
          file_content,
          login,
          password,
          encryptedPrivateKey,
          keyModule,
          hash_function_1,
        );
      }

      clearInterval(progressInterval);
      setProgress(100);

      setSignatureData({
        ...signed_file,
        algorithm,
        options: {
          saltLength: algorithm === "RSA" ? saltLength : undefined,
          hash_function_1,
          hash_function_2: algorithm === "RSA" ? hash_function_2 : undefined,
        },
      });
      setIsSigned(true);
    } catch (error) {
      console.error("API Error:", error);
      clearInterval(progressInterval);
      setProgress(0);
      setCalculated("Error during signing process.");
    } finally {
      setLoading(false);
    }
  };

  const handleDownloadZip = async () => {
    if (!selectedFile || !signatureData) return;

    const zip = new JSZip();

    zip.file(selectedFile.name, selectedFile);

    const metaData = {
      user: login,
      timestamp: new Date().toISOString(),
      originalFileName: selectedFile.name,
      algorithm: signatureData.algorithm,
      algorithmOptions: signatureData.options,
      signatureDetails: signatureData,
    };

    zip.file("signature_info.json", JSON.stringify(metaData, null, 2));

    try {
      const zipBytes = await zip.generateAsync({ type: "uint8array" });

      const filePath = await save({
        defaultPath: `Signed_${selectedFile.name.substring(0, selectedFile.name.lastIndexOf(".")) || selectedFile.name}.ss`,
        filters: [
          {
            name: "SecureSign Archive",
            extensions: ["ss"],
          }
        ],
      });

      if (filePath) {
        await writeFile(filePath, zipBytes);
        console.log("File successfully saved to:", filePath);
      }
    } catch (error) {
      console.error("Error saving SS file:", error);
    }
  };

  const handleFileChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    if (e.target.files && e.target.files.length > 0) {
      setSelectedFile(e.target.files[0]);
      setProgress(0);
      setIsSigned(false);
      setSignatureData(null);
    }
  };

  const handleDrop = (e: React.DragEvent<HTMLDivElement>) => {
    e.preventDefault();
    if (e.dataTransfer.files && e.dataTransfer.files.length > 0) {
      setSelectedFile(e.dataTransfer.files[0]);
      setProgress(0);
      setIsSigned(false);
      setSignatureData(null);
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
        Encrypt and Sign Your Files
      </p>
      <div className="bg-white p-12 rounded-2xl shadow-sm border border-slate-100 flex flex-col items-center justify-center min-h-[400px]">
        <input
          type="file"
          ref={fileInputRef}
          onChange={handleFileChange}
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
              <span className="font-bold text-[#0f172a]">SS FILE</span>
            </p>
          </div>
        ) : (
          <div className="flex flex-col items-center w-full max-w-md animate-in fade-in slide-in-from-bottom-4">
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

        {selectedFile && !isSigned && (
          <div className="mt-12 w-full max-w-2xl flex flex-col items-center border-t border-slate-100 pt-8 gap-4 animate-in fade-in slide-in-from-bottom-2">
            <div className="space-y-4 w-full max-w-md">
              <div className="flex flex-col space-y-1">
                <label className="text-xs font-semibold text-slate-500 uppercase">
                  Algorithm
                </label>
                <select
                  value={algorithm}
                  onChange={(e) => setAlgorithm(e.target.value)}
                  className="p-2.5 border border-slate-200 rounded-lg text-slate-700 bg-slate-50 outline-none focus:border-[#0f172a] transition-colors"
                >
                  <option value="RSA">RSA</option>
                  <option value="ElGamal">ElGamal</option>
                  <option value="ECDSA">ECDSA</option>
                </select>
              </div>

              {algorithm === "RSA" && (
                <>
                  <div className="flex flex-col space-y-1">
                    <label className="text-xs font-semibold text-slate-500 uppercase">
                      Salt Length
                    </label>
                    <input
                      type="number"
                      min="1"
                      max="256"
                      value={saltLength}
                      onChange={(e) => {
                        const value = Number(e.target.value);
                        if (value >= 1 && value <= 256) {
                          setSaltLength(value);
                        }
                      }}
                      className="p-2.5 border border-slate-200 rounded-lg text-slate-700 bg-slate-50 outline-none focus:border-[#0f172a] transition-colors"
                    />
                  </div>

                  <div className="flex flex-col space-y-1">
                    <label className="text-xs font-semibold text-slate-500 uppercase">
                      Hash Function
                    </label>
                    <select
                      value={hash_function_1}
                      onChange={(e) => setHash_function_1(e.target.value)}
                      className="p-2.5 border border-slate-200 rounded-lg text-slate-700 bg-slate-50 outline-none focus:border-[#0f172a] transition-colors"
                    >
                      <option value="SHA256">SHA256</option>
                      <option value="SHA3_256">SHA3_256</option>
                      <option value="SHA3_512">SHA3_512</option>
                    </select>
                  </div>

                  <div className="flex flex-col space-y-1">
                    <label className="text-xs font-semibold text-slate-500 uppercase">
                      MGF1 Hash
                    </label>
                    <select
                      value={hash_function_2}
                      onChange={(e) => setHash_function_2(e.target.value)}
                      className="p-2.5 border border-slate-200 rounded-lg text-slate-700 bg-slate-50 outline-none focus:border-[#0f172a] transition-colors"
                    >
                      <option value="SHA256">SHA256</option>
                      <option value="SHA3_256">SHA3_256</option>
                      <option value="SHA3_512">SHA3_512</option>
                    </select>
                  </div>
                </>
              )}

              {(algorithm === "ElGamal" || algorithm === "ECDSA") && (
                <div className="flex flex-col space-y-1">
                  <label className="text-xs font-semibold text-slate-500 uppercase">
                    Hash Function
                  </label>
                  <select
                    value={hash_function_1}
                    onChange={(e) => setHash_function_1(e.target.value)}
                    className="p-2.5 border border-slate-200 rounded-lg text-slate-700 bg-slate-50 outline-none focus:border-[#0f172a] transition-colors"
                  >
                    <option value="SHA256">SHA256</option>
                    <option value="SHA3_256">SHA3_256</option>
                    <option value="SHA3_512">SHA3_512</option>
                  </select>
                </div>
              )}
            </div>

            <EncryptAndSignBtn
              onClick={handleEncryptAndSign}
              disabled={loading}
            />
          </div>
        )}
      </div>

      {!isSigned ? (
        <TempResultSection data={calculated} />
      ) : (
        <div className="w-full flex flex-col items-center justify-center p-8 bg-green-50 border border-green-200 rounded-2xl shadow-sm animate-in fade-in slide-in-from-bottom-4">
          <p className="text-2xl font-bold text-green-800 mb-2">
            File Signed Successfully! 🎉
          </p>
          <p className="text-green-600 mb-6 text-center">
            Your file and signature have been securely generated.
          </p>
          <button
            onClick={handleDownloadZip}
            className="flex items-center cursor-pointer space-x-2 bg-[#0f172a] hover:bg-slate-800 text-white font-medium py-3 px-6 rounded-lg transition-colors shadow-sm"
          >
            <Download className="w-5 h-5" />
            <span>Download SS package</span>
          </button>
        </div>
      )}
    </div>
  );
};

export default EncryptPage;
