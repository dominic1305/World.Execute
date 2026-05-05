@echo off

@rem miniaudio
if not exist "miniaudio.h" (
	echo Downloading miniaudio.h
	curl -L -o "miniaudio.h" "https://raw.githubusercontent.com/mackron/miniaudio/refs/heads/master/miniaudio.h"
) else (
	echo miniaudio.h already installed
)

if not exist "miniaudio.c" (
	echo Downloading miniaudio.c
	curl -L -o "miniaudio.c" "https://raw.githubusercontent.com/mackron/miniaudio/refs/heads/master/miniaudio.c"
) else (
	echo miniaudio.c already installed
)
echo.

pause