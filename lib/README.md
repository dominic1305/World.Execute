# Library Downloader

This project uses a simple powershell script to automatically download required third-party libraries.

## Automatic Setup (Recommended)

1. Run `download.ps1`
2. The script will download and place all required libraries into the appropriate folders.

## Manual Setup (Fallback)

If the script fails, you can manually install the required libraries:

### Required Libraries

* miniaudio - download from: [github/miniaudio](https://github.com/mackron/miniaudio) [[docs](https://miniaud.io/docs/manual/index.html)]

### Installation Steps

1. Download the library
2. Extract it (if needed)
3. Place it in: `libs/[library directory]`

## Verification

After setup, ensure:
* The `libs/` folder contains the required files
* The project builds without missing dependency errors