mklink /D "%~dp0debug\Engine\Data" "%~dp0Data"
mklink /D "%~dp0debug\Editor\Data" "%~dp0Data"

mklink /D "%~dp0release\Engine\Data" "%~dp0Data"
mklink /D "%~dp0release\Editor\Data" "%~dp0Data"

@echo off

for %%f in ("%~dp0Dependencies\*.dll") do (
    mklink "%~dp0debug\Engine\%%~nf.dll" "%%~f"
)

for %%f in ("%~dp0Dependencies\*.dll") do (
    mklink "%~dp0debug\Editor\%%~nf.dll" "%%~f"
)

for %%f in ("%~dp0Dependencies\*.dll") do (
    mklink "%~dp0release\Engine\%%~nf.dll" "%%~f"
)

for %%f in ("%~dp0Dependencies\*.dll") do (
    mklink "%~dp0release\Editor\%%~nf.dll" "%%~f"
)
