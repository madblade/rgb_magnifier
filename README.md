# How to use

- Right-click on window: exit
- Middle-mouse click on window: change mode (1: RGB, 2: pixels, 3: pixels with borders)
- Mousewheel on window: zoom (3 levels)
- Moving the mouse anywhere refreshes the display
- To refresh without moving the mouse: bring window to focus and hit or hold any keyboard key
- Drag and drop window to move it around


# Build from source

### You need an AVX2-compatible processor

### Install portable Windows SDK from this gist:
https://gist.github.com/mmozeiko/7f3162ec2988e81e56d5c4e22cde9977

### To build with powershell
You can source SDK dirs with:
`.\msvc\setup_ps.ps1`

### Build in release configuration:
`cl .\rgb_magnifier.c /Wall /O2 /Oi- /GS- /GR- /EHsc /Gy /Os /MD /link /entry:Main /subsystem:windows /nodefaultlib /opt:ref /opt:icf /nologo /emitpogophaseinfo /nocoffgrpinfo /emittoolversioninfo:no /RELEASE user32.lib kernel32.lib gdi32.lib /out:rgb_magnifier.exe`

Note: /Oi- mandatory for /O2 and avoiding the CRT memset.

- Build in Debug + for PIX profiling:
`cl .\rgb_magnifier.c /O2 /Oi- /MD /GS- /Zi /link /entry:Main /subsystem:windows /nodefaultlib /DEBUG:FULL user32.lib kernel32.lib gdi32.lib /out:rgb_magnifier.exe`

# License

MIT

# Performance

This uses WinGDI which is the simplest way to get screen pixels.
BitBlt (>30%), GetDIBits, SetDIBits are the main bottlenecks.
