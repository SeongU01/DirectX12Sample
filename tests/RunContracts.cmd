@echo off
setlocal
pushd "%~dp0.."
if not defined VSCMD_VER (
  for /f "usebackq delims=" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do call "%%i\VC\Auxiliary\Build\vcvars64.bat" >nul
)
where cl >nul 2>nul
if errorlevel 1 goto failure
if not exist bin\Debug\Contracts mkdir bin\Debug\Contracts
for %%t in (RenderApiContract FpsCameraContract FpsCameraInputContract PbrContract) do (
  cl /nologo /std:c++20 /EHsc /MDd /D_DEBUG /DUNICODE /D_UNICODE /utf-8 /IDirectX12Sample /IDirectX12Sample\ThirdParty /IDirectX12Sample\ThirdParty\include tests\%%t.cpp bin\Debug\DirectX12Sample.lib user32.lib /Febin\Debug\Contracts\%%t.exe /Fobin\Debug\Contracts\%%t.obj /Fdbin\Debug\Contracts\%%t.pdb /link /OPT:NOICF /OPT:NOREF
  if errorlevel 1 goto failure
  bin\Debug\Contracts\%%t.exe
  if errorlevel 1 goto failure
)
popd
exit /b 0
:failure
popd
exit /b 1
