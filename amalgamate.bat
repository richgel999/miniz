@echo off
setlocal

if not exist "amalgamate_config.txt" (
echo Configuration file not found. Creating a default amalgamate_config.txt...
echo # Default Configuration >amalgamate_config.txt
echo compiler_path=C:\Path\To\Compiler >>amalgamate_config.txt
echo cmake_generator=MinGW Makefiles >>amalgamate_config.txt
echo Please edit amalgamate_config.txt to match your environment before rerunning this script.
goto end
)

for /f "tokens=1,* delims==" %%a in (amalgamate_config.txt) do (
if /i "%%a"=="compiler_path" set "compiler_path=%%b"
if /i "%%a"=="cmake_generator" set "cmake_generator=%%b"
)

if "%compiler_path%"=="" (
echo Compiler path not set in amalgamate_config.txt. Please specify the compiler path.
goto end
)

if "%cmake_generator%"=="" (
echo CMake generator not set in amalgamate_config.txt. Please specify the CMake generator.
goto end
)

echo Configuration Loaded:
echo Compiler Path: %compiler_path%
echo CMake Generator: %cmake_generator%
set path=%compiler_path%;%path%
if not exist amalgamation mkdir amalgamation
if not exist _build\amalgamation mkdir _build\amalgamation
cmake -H. -B_build -DAMALGAMATE_SOURCES=ON -G "%cmake_generator%" >>amalgamate_results.log 2>&1
xcopy /Y /I _build\amalgamation\miniz.* amalgamation\
copy /Y ChangeLog.md amalgamation\
copy /Y LICENSE amalgamation\
copy /Y readme.md amalgamation\
if not exist amalgamation\examples mkdir amalgamation\examples
xcopy /Y /I examples\* amalgamation\examples\

:end
endlocal
