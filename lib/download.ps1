# miniaudio {
if (-not(test-path -path "miniaudio" -pathtype container))
{
	new-item -path "miniaudio" -itemtype directory | out-null
}
set-location -path "miniaudio"

if (-not(test-path -path "miniaudio.h" -pathtype leaf))
{
	write-host "installing miniaudio.h"
	invoke-webrequest -uri "https://raw.githubusercontent.com/mackron/miniaudio/refs/heads/master/miniaudio.h" -outfile "miniaudio.h"
}
else
{
	write-host "miniaudio.h already installed"
}

if (-not(test-path -path "miniaudio.c" -pathtype leaf))
{
	write-host "installing miniaudio.c"
	invoke-webrequest -uri "https://raw.githubusercontent.com/mackron/miniaudio/refs/heads/master/miniaudio.c" -outfile "miniaudio.c"
}
else
{
	write-host "miniaudio.c already installed"
}

set-location -path ".."
# } miniaudio