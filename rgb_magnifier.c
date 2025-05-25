#pragma warning(disable: 4255)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#pragma warning(default: 4255)

// No Windows CRT -> need to provide memset.
#pragma warning(disable: 4100)
void *memset(void *dest, int c, size_t len)
{
    // char *ptr = (char*)dest;
    // while (len--) *ptr++ = (char)c;
    return dest;
}
#pragma warning(default: 4100)

typedef LONGLONG i64;
typedef DWORD u32;
typedef INT i32;
typedef BYTE byte; // u8 is already defined as something else
typedef UINT_PTR u32ptr;
typedef HANDLE handle;

#include <immintrin.h> // AVX/AVX2

// Process data in 32-byte chunks. Data needs to be 32B-aligned.
void fastMemcpy(void * __restrict pvDest, void * __restrict pvSrc, size_t nBytes)
{
    const __m256i *pSrc = (const __m256i*)pvSrc;
    __m256i *pDest = (__m256i*)pvDest;

    // Ensure nb_pixels_source * stretch_factor is fixed to 320.
    if (nBytes == 1280)
    {
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));

    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));

    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));

    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    _mm256_stream_si256(pDest++, _mm256_stream_load_si256(pSrc++));
    }

    // Every line is nb_px_src * strtch_fctr * 4 (channels) bytes.
    // If nb_pixels_source * stretch_factor is a multiple of 8, you can use
    // the following:
    // i64 nVects = nBytes / sizeof(*pSrc);
    // for (; nVects > 0; nVects--, pSrc++, pDest++)
    // {
    //    // NT store bypasses RFO/cache.
    //    const __m256i loaded = _mm256_stream_load_si256(pSrc); // avx2 VMOVNTDQA
    //    _mm256_stream_si256(pDest, loaded); // avx VMOVNTDQ
    // }

    _mm_sfence(); // because NT stores are weakly ordered with other memory operations.
}

__declspec(noinline) void fastMemzero(void *pvDest, size_t nBytes)
{
    // Use only if nb_pixels_source * stretch_factor is a multiple of 8.
    // No real need to unroll the loop, it's not a hotspot.
    __m256i *pDest = (__m256i*)pvDest;
    i64 nVects = nBytes / sizeof(__m256i);
    const __m256i zeroVec = _mm256_setzero_si256();
    for (; nVects > 0; nVects--, pDest++) {
        _mm256_stream_si256(pDest, zeroVec); // avx VMOVNTDQ
    }
    _mm_sfence();
}

// Params
// If you change them you should modify fastMemcpy accordingly.
u32 nb_pixels_source = 32;
u32 stretch_factor = 10; // => 320 px.

// Windows needs this to enable floating point operations
// (needed for SetTimer frequency).
// If you port to C++, wrap this inside extern "C" {}
u32 _fltused = 0;

// Handles
byte user_quit = 0;
handle hwnd_popup = NULL;
handle window_hdc = NULL;
handle screen_hdc = NULL;

// Mouse
u32 mode = 2;
u32 zoom = 1;
POINT drag_offset;

// Temporary pixel buffers
handle mem_hdc = NULL;
handle mem_hdc2 = NULL;
handle bitmap = NULL;
handle bitmap2 = NULL;
handle old_bitmap = NULL;
handle old_bitmap2 = NULL;
BITMAPINFO bmi = {0};
BITMAPINFO bmi2 = {0};
byte *screen_pixels;
byte *output_pixels;

void Loop(void)
{
    POINT p;
    if (!GetCursorPos(&p)) {return;}

    // capture window area into bitmap
    // this is the main performance bottleneck
    BitBlt(mem_hdc, 0, 0,
        nb_pixels_source, nb_pixels_source,
        screen_hdc,
        p.x-nb_pixels_source/2, p.y-nb_pixels_source/2,
        SRCCOPY);
    GetDIBits(mem_hdc, bitmap, 0,
        nb_pixels_source, screen_pixels,
        &bmi, DIB_RGB_COLORS);

    u32 s1 = 1+(stretch_factor-1)/3;
    u32 s2 = 2 * s1 - 1;

    u32 nb_pixels_dest = stretch_factor * nb_pixels_source;
    u32 scanline_size = 4 * nb_pixels_dest * sizeof(byte);
    u32 band_size = scanline_size * stretch_factor;

    // for each source scanline
    for (u32 i = 0; i < nb_pixels_source; ++i)
    {
        u32 output_offset = i * band_size;
        if (mode == 1 || mode == 2)
        {
            // blank first dest scanline
            fastMemzero(output_pixels + output_offset, scanline_size);
        }

        // compute next dest scanline
        for (u32 j = 0; j < nb_pixels_source; ++j)
        {
            u32 l = i * nb_pixels_source + j;
            byte b = screen_pixels[4*l+0];
            byte g = screen_pixels[4*l+1];
            byte r = screen_pixels[4*l+2];

            if (mode == 0) // no borders
            {
                u32 o_l = output_offset + 4 * (j * stretch_factor);
                for (u32 jj = 0; jj < stretch_factor; ++jj)
                {
                    u32 offset = o_l + jj * 4;
                    output_pixels[offset+0] = b;
                    output_pixels[offset+1] = g;
                    output_pixels[offset+2] = r;
                }
            }
            else if (mode == 1) // borders
            {
                u32 o_l = output_offset +  4 * (nb_pixels_dest + j * stretch_factor);
                output_pixels[o_l+0] = 0;
                output_pixels[o_l+1] = 0;
                output_pixels[o_l+2] = 0;
                for (u32 jj = 1; jj < stretch_factor; ++jj)
                {
                    u32 offset = o_l + jj * 4;
                    output_pixels[offset+0] = b;
                    output_pixels[offset+1] = g;
                    output_pixels[offset+2] = r;
                }
            }
            else if (mode == 2) // RGB mode
            {
                u32 o_l = output_offset + 4 * (nb_pixels_dest + j * stretch_factor);
                output_pixels[o_l+0] = 0;
                output_pixels[o_l+1] = 0;
                output_pixels[o_l+2] = 0;
                for (u32 jj = 1; jj < s1; ++jj)
                {
                    u32 offset = o_l + jj * 4;
                    output_pixels[offset+0] = 0;
                    output_pixels[offset+1] = 0;
                    output_pixels[offset+2] = r;
                }
                for (u32 jj = s1; jj < s2; ++jj)
                {
                    u32 offset = o_l + jj * 4;
                    output_pixels[offset+0] = 0;
                    output_pixels[offset+1] = g;
                    output_pixels[offset+2] = 0;
                }
                for (u32 jj = s2; jj < stretch_factor; ++jj)
                {
                    u32 offset = o_l + jj * 4;
                    output_pixels[offset+0] = b;
                    output_pixels[offset+1] = 0;
                    output_pixels[offset+2] = 0;
                }
            }
        }

        u32 startSource = i * band_size;
        u32 scanline = 1;
        if (mode > 0)
        {
            startSource += 4 * nb_pixels_dest;
            scanline = 2;
        }

        // copy+paste the previous dest scanline enough times to fill the rest of the band
        u32 current_scanline = 0;
        for (; scanline < stretch_factor; ++scanline)
        {
            current_scanline++;
            u32 startDest = startSource + scanline_size * current_scanline;
            fastMemcpy(
                output_pixels + startDest,
                output_pixels + startSource, scanline_size);
        }
    }

    // copy processed bitmap into buffer
    SetDIBits(mem_hdc2, bitmap2, 0, nb_pixels_dest,
        output_pixels, &bmi2, DIB_RGB_COLORS);

    // blit area back to window
    if (zoom == 1)
    {
        BitBlt(window_hdc, 0, 0, nb_pixels_dest,
            nb_pixels_dest, mem_hdc2, 0, 0, SRCCOPY);
    }
    else
    {
        u32 subzone_halfwidth = nb_pixels_dest >> zoom;
        StretchBlt(
            window_hdc, 0, 0, nb_pixels_dest,
            nb_pixels_dest,
            mem_hdc2,
            (nb_pixels_dest/2) - subzone_halfwidth,
            (nb_pixels_dest/2) - subzone_halfwidth,
            2*subzone_halfwidth, 2*subzone_halfwidth,
            SRCCOPY);
    }
}

LRESULT CALLBACK WndProc(HANDLE hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static u32ptr timer_id = 0;
    switch (msg)
    {
        case WM_CREATE:
        {
            DEVMODE dm = {0};
            dm.dmSize = sizeof(dm);
            EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
            u32 interval = (u32)(1000.0/dm.dmDisplayFrequency+0.5);
            timer_id = SetTimer(hwnd, 1, interval, NULL);
            return 0;
        }

        case WM_CLOSE:
        {
            DestroyWindow(hwnd_popup);
            return 0;
        }
        case WM_RBUTTONUP:
        case WM_DESTROY:
        {
            KillTimer(hwnd, timer_id);
            user_quit = 1;
            PostQuitMessage(0);
            return 0;
        }

        // Mode selection
        case WM_MOUSEWHEEL:
        {
            i32 delta = GET_WHEEL_DELTA_WPARAM(wParam);
            if (delta > 0) {
                zoom = zoom < 3 ? zoom + 1: zoom;
            } else if (delta < 0) {
                zoom = zoom > 1 ? zoom - 1 : zoom;
            }
            return 0;
        }
        case WM_MBUTTONDOWN:
        {
            mode = (mode+1)%3;
            return 0;
        }

        // Move window around
        case WM_LBUTTONDOWN:
        {
            SetCapture(hwnd_popup);
            drag_offset.x = LOWORD(lParam);
            drag_offset.y = HIWORD(lParam);
            return 0;
        }
        case WM_LBUTTONUP:
        {
            SetCapture(NULL);
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            if (GetCapture() == hwnd_popup)
            {
                POINT cursorPos;
                GetCursorPos(&cursorPos);
                SetWindowPos(hwnd_popup, NULL,
                    cursorPos.x - drag_offset.x,
                    cursorPos.y - drag_offset.y,
                    0, 0, SWP_NOSIZE | SWP_NOZORDER);
            }
            return 0;
        }

        // Force default cursor (no "spinning-loading" cursor)
        case WM_SETCURSOR:
        {
            if (LOWORD(lParam) == HTCLIENT)
            {
                SetCursor(LoadCursor(NULL, IDC_ARROW));
                return TRUE;
            }
            return 0;
        }

        // Draw
        case WM_TIMER:
        {
            InvalidateRect(hwnd, NULL, FALSE); // causes WM_PAINT
            return 0;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(hwnd, &ps);
            Loop();
            EndPaint(hwnd, &ps);
            return 0;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Main(void)
{
    handle hInstance = GetModuleHandle(NULL);

    WNDCLASSEX wc = { 
        .cbSize = sizeof(WNDCLASSEX),
        .lpfnWndProc = WndProc,
        .hInstance = hInstance,
        .hCursor = NULL,
        .lpszClassName = "RGBWindowClass"
    };
    RegisterClassEx(&wc);

    // Initialize 2x bitmap buffers: 32x32 and (10x32)*(10x32)

    u32 nb_pixels_dest = nb_pixels_source * stretch_factor;
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = nb_pixels_source;
    bmi.bmiHeader.biHeight = -1*nb_pixels_source; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    bmi2.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi2.bmiHeader.biWidth = nb_pixels_source*stretch_factor;
    bmi2.bmiHeader.biHeight = -1*nb_pixels_source*stretch_factor;
    bmi2.bmiHeader.biPlanes = 1;
    bmi2.bmiHeader.biBitCount = 32;
    bmi2.bmiHeader.biCompression = BI_RGB;

    screen_hdc = GetDC(NULL);
    mem_hdc = CreateCompatibleDC(screen_hdc);
    bitmap = CreateCompatibleBitmap(screen_hdc, nb_pixels_source, nb_pixels_source);
    old_bitmap = (HBITMAP)SelectObject(mem_hdc, bitmap);
    screen_pixels = (byte*)VirtualAlloc(
        NULL,
        nb_pixels_source * nb_pixels_source * 4,
        MEM_COMMIT | MEM_RESERVE, // committed pages are 0-initialized by default
        PAGE_READWRITE
    );
    
    mem_hdc2 = CreateCompatibleDC(screen_hdc);
    bitmap2 = CreateCompatibleBitmap(screen_hdc,
        nb_pixels_dest, nb_pixels_dest);
    old_bitmap2 = (HBITMAP)SelectObject(mem_hdc2, bitmap2);
    output_pixels = (byte*)VirtualAlloc(
        NULL,
        nb_pixels_dest * nb_pixels_dest * 4,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    // Create window

    POINT pt;
    GetCursorPos(&pt);
    hwnd_popup = CreateWindowEx(
        WS_EX_TOPMOST,
        "RGBWindowClass", "RGB Magnifier",
        WS_POPUP, // Replace with WS_OVERLAPPEDWINDOW to get menu bar.
        pt.x, pt.y,
        nb_pixels_dest, nb_pixels_dest,
        NULL, NULL, hInstance, NULL
    );
    ShowWindow(hwnd_popup, SW_SHOW);
    UpdateWindow(hwnd_popup);
    window_hdc = GetWindowDC(hwnd_popup);

    // To use an icon (keep in mind there is no option for nearest filtering in GDI):
    // - Make a file icon.rc that contains: 101 ICON "icon.ico"
    // - Build with 'rc icon.rc'
    // - Add the following here:
    // handle hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(101));
    // SendMessage(hwnd_popup, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
    // SendMessage(hwnd_popup, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
    // - Add the generated file icon.res in the link arguments.
    // This will add at least 2KiB to the executable.

    // Window loop

    MSG msg;
    while (!user_quit)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        WaitMessage();
    }

    // Cleanup

    SelectObject(mem_hdc, old_bitmap);
    DeleteObject(bitmap);
    DeleteDC(mem_hdc);
    SelectObject(mem_hdc2, old_bitmap2);
    DeleteObject(bitmap2);
    DeleteDC(mem_hdc2);
    ReleaseDC(NULL, screen_hdc);
    VirtualFree(screen_pixels, 0, MEM_RELEASE);
    VirtualFree(output_pixels, 0, MEM_RELEASE);

    ExitProcess(0);
}
