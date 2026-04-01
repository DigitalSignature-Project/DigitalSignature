import React from 'react';
import { NavLink, Outlet } from 'react-router-dom';
import { Home, Lock, ShieldCheck, Settings, Bell, User, Shield } from 'lucide-react';

const Layout = () => {
  const navItems = [
    { name: 'Dashboard', path: '/', icon: Home },
    { name: 'Encrypt and Sign', path: '/encrypt', icon: Lock },
    { name: 'Verify Signature', path: '/verify', icon: ShieldCheck },
    { name: 'Settings', path: '/settings', icon: Settings },
  ];

  return (
    <div className="flex h-screen bg-slate-50 font-sans">
      <div className="w-64 bg-[#0f172a] text-white flex flex-col">
        <div className="h-16 flex items-center px-6 border-b border-slate-800">
          <Shield className="w-6 h-6 text-cyan-400 mr-3" />
          <span className="text-xl font-bold tracking-wide">SecureSign</span>
        </div>
        
        <nav className="flex-1 py-6 px-3 space-y-1">
          {navItems.map((item) => (
            <NavLink
              key={item.name}
              to={item.path}
              className={({ isActive }) =>
                `flex items-center px-4 py-3 rounded-lg transition-colors ${
                  isActive 
                    ? 'bg-slate-800/80 text-cyan-400 border-l-4 border-cyan-400' 
                    : 'text-slate-400 hover:bg-slate-800 hover:text-slate-200'
                }`
              }
            >
              <item.icon className="w-5 h-5 mr-3" />
              <span className="font-medium">{item.name}</span>
            </NavLink>
          ))}
        </nav>
      </div>

      <div className="flex-1 flex flex-col overflow-hidden">
        <header className="h-16 bg-white border-b border-slate-200 flex items-center justify-end px-8">
          <div className="flex items-center space-x-4">
            <button className="text-slate-400 hover:text-slate-600">
              <Bell className="w-5 h-5" />
            </button>
            <div className="flex items-center space-x-2 border-l pl-4 border-slate-200">
              <div className="w-8 h-8 bg-slate-200 rounded-full flex items-center justify-center">
                <User className="w-5 h-5 text-slate-500" />
              </div>
              <span className="text-sm font-medium text-slate-700">John Kowalski</span>
            </div>
          </div>
        </header>

        <main className="flex-1 overflow-y-auto p-8">
          <Outlet />
        </main>
      </div>
    </div>
  );
};

export default Layout;