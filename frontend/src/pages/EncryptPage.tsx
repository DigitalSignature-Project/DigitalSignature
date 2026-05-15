import { Plus, FileText } from "lucide-react";
import { EncryptAndSignBtn } from "../components/EncryptAndSignBtn";
import { TempResultSection } from "../components/TempResultSection";
import { useState, useRef } from "react";
import { signFile } from "../services/serverAPI"; // Upewnij się, że ścieżka jest poprawna

const EncryptPage = () => {
  const [loading, setLoading] = useState<boolean>(false);
  const [calculated, setCalculated] = useState<string>("Ready to sign");
  
  const [selectedFile, setSelectedFile] = useState<File | null>(null);
  const [progress, setProgress] = useState<number>(0);
  const [algorithm, setAlgorithm] = useState<string>("algo1");
  const [hashType, setHashType] = useState<string>("hash1");
  const fileInputRef = useRef<HTMLInputElement>(null);

const handleEncryptAndSign = async () => {
    if (!selectedFile) return;

    const filePath = (selectedFile as any).path;

    if (!filePath) {
      setCalculated("Błąd: Nie można odczytać ścieżki pliku na tym urządzeniu.");
      return;
    }
    
    setLoading(true);
    setCalculated("Signing file locally...");
    setProgress(0);
    
    const progressInterval = setInterval(() => {
      setProgress(p => {
        if (p < 50) return p + 6;
        if (p < 80) return p + 3;
        if (p < 90) return p + 1;
        return 90;
      });
    }, 150);

    try {
      const blob = await signFile(filePath, algorithm, hashType);
      
      const downloadUrl = window.URL.createObjectURL(blob);
      const link = document.createElement("a");
      link.href = downloadUrl;
      link.download = `signed_${selectedFile.name}`;
      document.body.appendChild(link);
      link.click();
      link.remove();
      window.URL.revokeObjectURL(downloadUrl);

      clearInterval(progressInterval);
      setProgress(100);
      setCalculated("Success! File signed and downloaded.");
    } catch (error) {
      console.error("Błąd API:", error);
      clearInterval(progressInterval);
      setProgress(0);
      setCalculated("Error during signing process.");
    } finally {
      setLoading(false);
    }
  };

  const handleFileChange = (e: React.ChangeEvent<HTMLInputElement>) => {
    if (e.target.files && e.target.files.length > 0) {
      setSelectedFile(e.target.files[0]);
      setProgress(0);
      setCalculated("Ready to sign");
    }
  };

  const handleDrop = (e: React.DragEvent<HTMLDivElement>) => {
    e.preventDefault();
    if (e.dataTransfer.files && e.dataTransfer.files.length > 0) {
      setSelectedFile(e.dataTransfer.files[0]);
      setProgress(0);
      setCalculated("Ready to sign");
    }
  };

  const handleDragOver = (e: React.DragEvent<HTMLDivElement>) => {
    e.preventDefault();
  };

  const formatBytes = (bytes: number) => (bytes / (1024 * 1024)).toFixed(2) + " MB";

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
              Drag and drop file here or
              <br />
              <span className="font-bold text-[#0f172a]">CLICK TO CHOOSE</span>
            </p>
          </div>
        ) : (
          <div className="flex flex-col items-center w-full max-w-md animate-in fade-in slide-in-from-bottom-4">
            <div className="flex items-center space-x-4 cursor-pointer hover:opacity-80 transition-opacity w-full" onClick={() => fileInputRef.current?.click()}>
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
                <span className="text-xs text-cyan-600 font-semibold block">Click to change file</span>
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
          <div className="mt-12 w-full max-w-2xl flex items-end justify-between border-t border-slate-100 pt-8 gap-4 animate-in fade-in slide-in-from-bottom-2">
            <div className="space-y-4 flex-1">
              <div className="flex flex-col space-y-1">
                <label className="text-xs font-semibold text-slate-500 uppercase">Algorithm</label>
                <select 
                  value={algorithm}
                  onChange={(e) => setAlgorithm(e.target.value)}
                  className="p-2.5 border border-slate-200 rounded-lg text-slate-700 bg-slate-50 outline-none focus:border-[#0f172a] transition-colors"
                >
                  <option value="algo1">Work in progress (Option 1)</option>
                  <option value="algo2">Work in progress (Option 2)</option>
                  <option value="algo3">Work in progress (Option 3)</option>
                </select>
              </div>
              
              <div className="flex flex-col space-y-1">
                <label className="text-xs font-semibold text-slate-500 uppercase">Hash Type</label>
                <select 
                  value={hashType}
                  onChange={(e) => setHashType(e.target.value)}
                  className="p-2.5 border border-slate-200 rounded-lg text-slate-700 bg-slate-50 outline-none focus:border-[#0f172a] transition-colors"
                >
                  <option value="hash1">Work in progress (Hash 1)</option>
                  <option value="hash2">Work in progress (Hash 2)</option>
                  <option value="hash3">Work in progress (Hash 3)</option>
                </select>
              </div>
            </div>

            <EncryptAndSignBtn
              onClick={handleEncryptAndSign}
              disabled={loading}
            />
          </div>
        )}
      </div>

      <TempResultSection data={calculated} />
    </div>
  );
};

export default EncryptPage;