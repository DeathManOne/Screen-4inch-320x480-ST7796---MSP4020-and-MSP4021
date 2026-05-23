@REM
@REM dragYourFileOverMe.bat
@REM
@REM Copyright (c) 2026 DeathManOne
@REM https://github.com/DeathManOne
@REM
@REM This file is part of the ST7796S library.
@REM
@REM ST7796S is free software: you can redistribute it and/or modify
@REM it under the terms of the GNU General Public License as published by
@REM the Free Software Foundation, either version 3 of the License, or
@REM (at your option) any later version.
@REM
@REM ST7796S is distributed in the hope that it will be useful,
@REM but WITHOUT ANY WARRANTY; without even the implied warranty of
@REM MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
@REM See the GNU General Public License for more details.
@REM
@REM You should have received a copy of the GNU General Public License
@REM along with ST7796S.
@REM If not, see <https://www.gnu.org/licenses/>.
@REM

@echo off
setlocal enabledelayedexpansion

if "%~1"=="" (
    echo Drag ^& drop images or fonts here
    pause
    exit /b
)

for %%F in (%*) do (
    set "ext=%%~xF"
    set "ext=!ext:~1!"
    set "ext=!ext!"

    if /I "!ext!"=="ttf" (
        python "%~dp0font2h.py" "%%~fF"
    ) else if /I "!ext!"=="otf" (
        python "%~dp0font2h.py" "%%~fF"
    ) else if /I "!ext!"=="png" (
        python "%~dp0image2h.py" "%%~fF"
    ) else if /I "!ext!"=="jpg" (
        python "%~dp0image2h.py" "%%~fF"
    ) else if /I "!ext!"=="jpeg" (
        python "%~dp0image2h.py" "%%~fF"
    ) else if /I "!ext!"=="bmp" (
        python "%~dp0image2h.py" "%%~fF"
    ) else if /I "!ext!"=="webp" (
        python "%~dp0image2h.py" "%%~fF"
    ) else if /I "!ext!"=="gif" (
        python "%~dp0image2h.py" "%%~fF"
    ) else (
        echo Unsupported file: %%~nxF
    )
)

echo.
echo DONE
pause