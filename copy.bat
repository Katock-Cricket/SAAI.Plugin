@echo off

REM check GTA_SA_DIR
if "%GTA_SA_DIR%"=="" (
    echo GTA_SA_DIR not set。
    exit /b 1
)

REM copy
set SOURCE_PATH=cmake-build-release\saai.asi
copy "%SOURCE_PATH%" "%GTA_SA_DIR%\scripts\"

REM check success
if errorlevel 1 (
    echo failed
    exit /b 1
) else (
    echo success
)

exit /b 0
