Set-Location BakkesPlugin
git clone https://github.com/Microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg install fmt:x64-windows-static
.\vcpkg\vcpkg install imgui[core,dx12-binding,win32-binding]:x64-windows-static
.\vcpkg\vcpkg integrate install
Set-Location ..