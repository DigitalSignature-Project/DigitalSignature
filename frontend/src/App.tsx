import { BrowserRouter, Routes, Route } from "react-router-dom";
import Layout from "./pages/Layout";
import EncryptPage from "./pages/EncryptPage";
import VerifyPage from "./pages/VerifyPage";

const App = () => {
  return (
    <BrowserRouter>
      <Routes>
        <Route path="/" element={<Layout />}>
          <Route
            index
            element={
              <div className="text-2xl font-bold text-slate-800">
                This will be the Dashboard
              </div>
            }
          />
          <Route path="encrypt" element={<EncryptPage />} />
          <Route path="verify" element={<VerifyPage />} />
          <Route
            path="settings"
            element={
              <div className="text-2xl font-bold text-slate-800">
                Settings will be here
              </div>
            }
          />
        </Route>
      </Routes>
    </BrowserRouter>
  );
};

export default App;
