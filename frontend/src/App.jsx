import React from 'react';
import { BrowserRouter, Routes, Route, Navigate } from 'react-router-dom';
import Layout from './Layout';
import EncryptPage from './EncryptPage';

const App = () => {
  return (
    <BrowserRouter>
      <Routes>
        <Route path="/" element={<Layout />}>
          <Route index element={<div className="text-2xl font-bold text-slate-800">This will be the Dashboard</div>} />
          <Route path="encrypt" element={<EncryptPage />} />
          <Route path="verify" element={<div className="text-2xl font-bold text-slate-800">Verification will be here</div>} />
          <Route path="settings" element={<div className="text-2xl font-bold text-slate-800">Settings will be here</div>} />
        </Route>
      </Routes>
    </BrowserRouter>
  );
};

export default App;