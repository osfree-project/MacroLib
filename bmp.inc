BITMAPFILEHEADER struct
    bfType      dw ?    ; "BM" signature (0x4D42)
    bfSize      dd ?    ; File size in bytes
    bfReserved1 dw ?    ; Reserved (0)
    bfReserved2 dw ?    ; Reserved (0)
    bfOffBits   dd ?    ; Offset to pixel data
BITMAPFILEHEADER ends

BITMAPINFOHEADER struct
    biSize          dd ?    ; Size of this header (40 bytes)
    biWidth         dd ?    ; Image width in pixels
    biHeight        dd ?    ; Image height in pixels
    biPlanes        dw ?    ; Number of color planes (1)
    biBitCount      dw ?    ; Bits per pixel (16 for 16-bit BMP)
    biCompression   dd ?    ; Compression type (BI_RGB for uncompressed)
    biSizeImage     dd ?    ; Size of image data in bytes
    biXPelsPerMeter dd ?    ; Horizontal resolution
    biYPelsPerMeter dd ?    ; Vertical resolution
    biClrUsed       dd ?    ; Number of colors in the palette (0 for 16-bit)
    biClrImportant  dd ?    ; Number of important colors (0)
BITMAPINFOHEADER ends
