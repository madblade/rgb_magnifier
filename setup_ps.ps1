$env:VSCMD_ARG_HOST_ARCH='x64'
$env:VSCMD_ARG_TGT_ARCH='x64'

$env:VCToolsVersion='14.43.34808'
$env:WindowsSDKVersion='10.0.26100.0\'

$env:VCToolsInstallDir=$PSScriptRoot+'\VC\Tools\MSVC\14.43.34808\'
$env:WindowsSdkBinPath=$PSScriptRoot+'\Windows Kits\10\bin\'

$env:PATH=
$PSScriptRoot+'\VC\Tools\MSVC\14.43.34808\bin\Hostx64\x64;'+
$PSScriptRoot+'\Windows Kits\10\bin\10.0.26100.0\x64;'+
$PSScriptRoot+'\Windows Kits\10\bin\10.0.26100.0\x64\ucrt;'+$env:PATH

$env:INCLUDE=
$PSScriptRoot+'\VC\Tools\MSVC\14.43.34808\include;'+
$PSScriptRoot+'\Windows Kits\10\Include\10.0.26100.0\ucrt;'+
$PSScriptRoot+'\Windows Kits\10\Include\10.0.26100.0\shared;'+
$PSScriptRoot+'\Windows Kits\10\Include\10.0.26100.0\um;'+
$PSScriptRoot+'\Windows Kits\10\Include\10.0.26100.0\winrt;'+
$PSScriptRoot+'\Windows Kits\10\Include\10.0.26100.0\cppwinrt'

$env:LIB=
$PSScriptRoot+'\VC\Tools\MSVC\14.43.34808\lib\x64;'+
$PSScriptRoot+'\Windows Kits\10\Lib\10.0.26100.0\ucrt\x64;'+
$PSScriptRoot+'\Windows Kits\10\Lib\10.0.26100.0\um\x64'
