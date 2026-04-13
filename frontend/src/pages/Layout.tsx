import { useState, useEffect } from 'react';
import { NavLink, Outlet, useNavigate } from 'react-router-dom'; 
import { Home, Lock, ShieldCheck, Settings, Bell, User, Shield, LogOut, ChevronDown } from 'lucide-react';
import { getCurrentWindow, LogicalSize } from '@tauri-apps/api/window';

const Layout = () => {
  const navigate = useNavigate(); 
  const [isProfileMenuOpen, setIsProfileMenuOpen] = useState(false);

  useEffect(() => {
    const expandWindow = async () => {
      try {
        const appWindow = getCurrentWindow();
        
        await appWindow.setResizable(true);
        await appWindow.setSize(new LogicalSize(1200, 800));
        await appWindow.center();
        
        console.log("Window expanded and unlocked successfully");
      } catch (error) {
        console.error("Failed to expand window:", error);
      }
    };

    if ((window as any).__TAURI_INTERNALS__) {
      expandWindow();
    }
  }, []);

  const navItems = [
    { name: 'Dashboard', path: '/', icon: Home },
    { name: 'Encrypt and Sign', path: '/encrypt', icon: Lock },
    { name: 'Verify Signature', path: '/verify', icon: ShieldCheck },
    { name: 'Settings', path: '/settings', icon: Settings },
  ];

  const handleLogout = () => {
    localStorage.removeItem('isAuthenticated');
    sessionStorage.removeItem('isAuthenticated');
    setIsProfileMenuOpen(false);
    navigate('/login');
  };

  const handleTestForget = () => {
    localStorage.removeItem('isAuthenticated');
    sessionStorage.removeItem('isAuthenticated');
    setIsProfileMenuOpen(false);
    navigate('/login');
  };

  return (
    <div className="flex h-screen bg-slate-50 font-sans">
      <div className="w-64 bg-[#0f172a] text-white flex flex-col z-10">
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

      <div className="flex-1 flex flex-col overflow-hidden relative">
        <header className="h-16 bg-white border-b border-slate-200 flex items-center justify-end px-8 z-20">
          <div className="flex items-center space-x-4">
            <button className="text-slate-400 hover:text-slate-600">
              <Bell className="w-5 h-5" />
            </button>
            
            <div className="relative">
              <div 
                className="flex items-center space-x-2 border-l pl-4 border-slate-200 cursor-pointer hover:opacity-80"
                onClick={() => setIsProfileMenuOpen(!isProfileMenuOpen)} 
              >
                <div className="w-8 h-8 bg-slate-200 rounded-full flex items-center justify-center">
                  <User className="w-5 h-5 text-slate-500" />
                </div>
                <span className="text-sm font-medium text-slate-700">John Kowalski</span>
                <ChevronDown className={`w-4 h-4 text-slate-400 transition-transform duration-200 ${isProfileMenuOpen ? 'rotate-180' : ''}`} />
              </div>

              {isProfileMenuOpen && (
                <div className="absolute right-0 mt-3 w-48 bg-white rounded-xl shadow-lg border border-slate-100 py-2 animate-in fade-in slide-in-from-top-2">
                  <button 
                    onClick={handleTestForget}
                    className="w-full text-left px-4 py-2 text-sm text-slate-700 hover:bg-slate-50 hover:text-cyan-600 transition-colors font-semibold"
                  >
                    Zapomnij dane logowania (dla testów!)
                  </button>
                  <button 
                    onClick={() => setIsProfileMenuOpen(false)}
                    className="w-full text-left px-4 py-2 text-sm text-slate-700 hover:bg-slate-50 hover:text-cyan-600 transition-colors"
                  >
                    Option 2
                  </button>
                  <div className="h-px bg-slate-100 my-1"></div>
                  <button 
                    onClick={handleLogout}
                    className="w-full text-left px-4 py-2 text-sm text-red-600 hover:bg-red-50 hover:text-red-700 transition-colors flex items-center"
                  >
                    <LogOut className="w-4 h-4 mr-2" />
                    Log out
                  </button>
                </div>
              )}
            </div>

          </div>
        </header>

        <main className="flex-1 overflow-y-auto p-8 z-0 relative">
          {isProfileMenuOpen && (
            <div 
              className="absolute inset-0 z-10" 
              onClick={() => setIsProfileMenuOpen(false)}
            />
          )}
          
          <div className="relative z-20">
            <Outlet />
          </div>
        </main>
      </div>
    </div>
  );
};

export default Layout;