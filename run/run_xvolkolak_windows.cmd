@echo off
setlocal

for %%I in ("%~dp0..") do set "PROJECT_ROOT=%%~fI"
set "APP_EXE=%PROJECT_ROOT%\tmp_build\src\gui\xvolkolak.exe"

if not exist "%APP_EXE%" (
    set "APP_EXE=%PROJECT_ROOT%\tmp_build\src\gui\Release\xvolkolak.exe"
)

if not exist "%APP_EXE%" (
    echo xvolkolak.exe was not found under tmp_build.
    exit /b 1
)

"%APP_EXE%" %*

endlocal
