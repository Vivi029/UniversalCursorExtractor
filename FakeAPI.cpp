/*
Copyright (C) 2004 Jacquelin POTIER <jacquelin.potier@free.fr>
Dynamic aspect ratio code Copyright (C) 2004 Jacquelin POTIER <jacquelin.potier@free.fr>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

// to avoid rebasing, use the linker/advanced/BaseAddress option 

#include "../../_Common_Files/GenericFakeAPI.h"
// You just need to edit this file to add new fake api 
// WARNING YOUR FAKE API MUST HAVE THE SAME PARAMETERS AND CALLING CONVENTION AS THE REAL ONE,
//                  ELSE YOU WILL GET STACK ERRORS

///////////////////////////////////////////////////////////////////////////////
// fake API prototype MUST HAVE THE SAME PARAMETERS 
// for the same calling convention see MSDN : 
// "Using a Microsoft modifier such as __cdecl on a data declaration is an outdated practice"
///////////////////////////////////////////////////////////////////////////////

int WINAPI mMessageBoxW(HWND hWnd,LPCWSTR lpText,LPCWSTR lpCaption,UINT uType);
int WINAPI mMessageBoxA(HWND hWnd,LPCSTR lpText,LPCSTR lpCaption,UINT uType);
HCURSOR mSetCursor(HCURSOR hCursor);

///////////////////////////////////////////////////////////////////////////////
// fake API array. Redirection are defined here
///////////////////////////////////////////////////////////////////////////////
STRUCT_FAKE_API pArrayFakeAPI[]=
{
    // library name ,function name, function handler, stack size (required to allocate enough stack space), FirstBytesCanExecuteAnywhereSize (optional put to 0 if you don't know it's meaning)
    //                                                stack size= sum(StackSizeOf(ParameterType))           Same as monitoring file keyword (see monitoring file advanced syntax)
    {_T("User32.dll"),_T("MessageBoxA"),(FARPROC)mMessageBoxA,StackSizeOf(HWND)+StackSizeOf(LPCSTR)+StackSizeOf(LPCSTR)+StackSizeOf(UINT),0},
    {_T("User32.dll"),_T("MessageBoxW"),(FARPROC)mMessageBoxW,StackSizeOf(HWND)+StackSizeOf(LPCWSTR)+StackSizeOf(LPCWSTR)+StackSizeOf(UINT),0},
    {_T("User32.dll"),_T("SetCursor"),(FARPROC)mSetCursor,StackSizeOf(HCURSOR),0},
    {_T(""),_T(""),NULL,0,0}// last element for ending loops
};

///////////////////////////////////////////////////////////////////////////////
// Before API call array. Redirection are defined here
///////////////////////////////////////////////////////////////////////////////
STRUCT_FAKE_API_WITH_USERPARAM pArrayBeforeAPICall[]=
{
    // library name ,function name, function handler, stack size (required to allocate enough stack space), FirstBytesCanExecuteAnywhereSize (optional put to 0 if you don't know it's meaning),userParam : a value that will be post back to you when your hook will be called
    //                                                stack size= sum(StackSizeOf(ParameterType))           Same as monitoring file keyword (see monitoring file advanced syntax)
    {_T(""),_T(""),NULL,0,0,0}// last element for ending loops
};

///////////////////////////////////////////////////////////////////////////////
// After API call array. Redirection are defined here
///////////////////////////////////////////////////////////////////////////////
STRUCT_FAKE_API_WITH_USERPARAM pArrayAfterAPICall[]=
{
    // library name ,function name, function handler, stack size (required to allocate enough stack space), FirstBytesCanExecuteAnywhereSize (optional put to 0 if you don't know it's meaning),userParam : a value that will be post back to you when your hook will be called
    //                                                stack size= sum(StackSizeOf(ParameterType))           Same as monitoring file keyword (see monitoring file advanced syntax)
    {_T(""),_T(""),NULL,0,0,0}// last element for ending loops
};
///////////////////////////////////////////////////////////////////////////////
///////////////////////////// NEW API DEFINITION //////////////////////////////
/////////////////////// You don't need to export these functions //////////////
///////////////////////////////////////////////////////////////////////////////

int WINAPI mMessageBoxA(HWND hWnd,LPCSTR lpText,LPCSTR lpCaption,UINT uType)
{
    UNREFERENCED_PARAMETER(lpCaption);
    char szMsg[2*MAX_PATH];

    strcpy(szMsg,"Oups !! It's not your text !!!\r\n[Begin of] Original Text :\r\n");
    if (lpText)
    {
        strncat(szMsg,lpText,MAX_PATH-strlen(szMsg)-1);
        szMsg[MAX_PATH-1]=0;// assume end of string
    }
    return MessageBoxA(hWnd,szMsg,"API Override:MessageBoxA",uType);
}


int WINAPI mMessageBoxW(HWND hWnd,LPCWSTR lpText,LPCWSTR lpCaption,UINT uType)
{
    UNREFERENCED_PARAMETER(lpCaption);
    wchar_t szMsg[2*MAX_PATH];

    wcscpy(szMsg,L"Oups !! It's not your text !!!\r\n[Begin of] Original Text :\r\n");
    if (lpText)
    {
        wcsncat(szMsg,lpText,MAX_PATH-wcslen(szMsg)-1);
        szMsg[MAX_PATH-1]=0;// assume end of string
    }

    return MessageBoxW(hWnd,szMsg,L"API Override:mMessageBoxW",uType);
}


/*
void something(HICON hIcon) {
    HBITMAP hBITMAPcopy;
    ICONINFOEX IconInfo;
    BITMAP BM_32_bit_color;
    BITMAP BM_1_bit_mask;

    // 1. From HICON to HBITMAP for color and mask separately
    //.cbSize required
    //memset((void*)&IconInfo, 0, sizeof(ICONINFOEX));
    IconInfo.cbSize = sizeof(ICONINFOEX);
    GetIconInfoEx(hIcon, &IconInfo);


    //HBITMAP IconInfo.hbmColor is 32bit per pxl, however alpha bytes can be zeroed or can be not.
    //HBITMAP IconInfo.hbmMask is 1bit per pxl

    // 2. From HBITMAP to BITMAP for color
    //    (HBITMAP without raw data -> HBITMAP with raw data)
    //         LR_CREATEDIBSECTION - DIB section will be created,
    //         so .bmBits pointer will not be null
    hBITMAPcopy = (HBITMAP)CopyImage(IconInfo.hbmColor, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
    //    (HBITMAP to BITMAP)
    GetObject(hBITMAPcopy, sizeof(BITMAP), &BM_32_bit_color);
    //Now: BM_32_bit_color.bmBits pointing to BGRA data.(.bmWidth * .bmHeight * (.bmBitsPixel/8))

    // 3. From HBITMAP to BITMAP for mask
    hBITMAPcopy = (HBITMAP)CopyImage(IconInfo.hbmMask, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);
    GetObject(hBITMAPcopy, sizeof(BITMAP), &BM_1_bit_mask);
    HANDLE hf;                 // file handle  
    BITMAPFILEHEADER hdr;       // bitmap file-header  
    PBITMAPINFOHEADER pbih;     // bitmap info-header  
    LPBYTE lpBits;              // memory pointer  
    DWORD dwTotal;              // total count of bytes  
    DWORD cb;                   // incremental count of bytes  
    BYTE* hp;                   // byte pointer  
    DWORD dwTmp;

    pbih = (PBITMAPINFOHEADER)BM_32_bit_color.;
    lpBits = (LPBYTE)GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    if (!lpBits)
        errhandler("GlobalAlloc", hwnd);

    // Retrieve the color table (RGBQUAD array) and the bits  
    // (array of palette indices) from the DIB.  
    if (!GetDIBits(hDC, hBMP, 0, (WORD)pbih->biHeight, lpBits, pbi,
        DIB_RGB_COLORS))
    {
        errhandler("GetDIBits", hwnd);
    }

    // Create the .BMP file.  
    hf = CreateFile(pszFile,
        GENERIC_READ | GENERIC_WRITE,
        (DWORD)0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        (HANDLE)NULL);
    if (hf == INVALID_HANDLE_VALUE)
        errhandler("CreateFile", hwnd);
    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M"  
    // Compute the size of the entire file.  
    hdr.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) +
        pbih->biSize + pbih->biClrUsed
        * sizeof(RGBQUAD) + pbih->biSizeImage);
    hdr.bfReserved1 = 0;
    hdr.bfReserved2 = 0;

    // Compute the offset to the array of color indices.  
    hdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) +
        pbih->biSize + pbih->biClrUsed
        * sizeof(RGBQUAD);

    // Copy the BITMAPFILEHEADER into the .BMP file.  
    if (!WriteFile(hf, (LPVOID)&hdr, sizeof(BITMAPFILEHEADER),
        (LPDWORD)&dwTmp, NULL))
    {
        errhandler("WriteFile", hwnd);
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file.  
    if (!WriteFile(hf, (LPVOID)pbih, sizeof(BITMAPINFOHEADER)
        + pbih->biClrUsed * sizeof(RGBQUAD),
        (LPDWORD)&dwTmp, (NULL)))
        errhandler("WriteFile", hwnd);

    // Copy the array of color indices into the .BMP file.  
    dwTotal = cb = pbih->biSizeImage;
    hp = lpBits;
    if (!WriteFile(hf, (LPSTR)hp, (int)cb, (LPDWORD)&dwTmp, NULL))
        errhandler("WriteFile", hwnd);

    // Close the .BMP file.  
    if (!CloseHandle(hf))
        errhandler("CloseHandle", hwnd);

    // Free memory.  
    GlobalFree((HGLOBAL)lpBits);
}*/

HRESULT SaveIcon(HICON hIcon, const wchar_t* path) {
    // Create the IPicture intrface
    PICTDESC desc = { sizeof(PICTDESC) };
    desc.picType = PICTYPE_ICON;
    desc.icon.hicon = hIcon;
    IPicture* pPicture = 0;
    HRESULT hr = OleCreatePictureIndirect(&desc, IID_IPicture, FALSE, (void**)&pPicture);
    if (FAILED(hr)) return hr;

    // Create a stream and save the image
    IStream* pStream = 0;
    CreateStreamOnHGlobal(0, TRUE, &pStream);
    LONG cbSize = 0;
    hr = pPicture->SaveAsFile(pStream, TRUE, &cbSize);

    // Write the stream content to the file
    if (!FAILED(hr)) {
        HGLOBAL hBuf = 0;
        GetHGlobalFromStream(pStream, &hBuf);
        void* buffer = GlobalLock(hBuf);
        HANDLE hFile = CreateFile(path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
        if (!hFile) hr = HRESULT_FROM_WIN32(GetLastError());
        else {
            DWORD written = 0;
            WriteFile(hFile, buffer, cbSize, &written, 0);
            CloseHandle(hFile);
        }
        GlobalUnlock(buffer);
    }
    // Cleanup
    pStream->Release();
    pPicture->Release();
    return hr;

}

static CLSID g_pngClsid = GUID_NULL;

// http://msdn.microsoft.com/en-us/library/windows/desktop/ms533843(v=vs.85).aspx
extern int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);


struct BITMAP_AND_BYTES {
    Gdiplus::Bitmap* bmp;
    int32_t* bytes;
};

static BITMAP_AND_BYTES createAlphaChannelBitmapFromIcon(HICON hIcon) {

    // Get the icon info
    ICONINFO iconInfo = { 0 };
    GetIconInfo(hIcon, &iconInfo);

    // Get the screen DC
    HDC dc = GetDC(NULL);

    // Get icon size info
    BITMAP bm = { 0 };
    GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bm);

    // Set up BITMAPINFO
    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bm.bmWidth;
    bmi.bmiHeader.biHeight = -bm.bmHeight;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    // Extract the color bitmap
    int nBits = bm.bmWidth * bm.bmHeight;
    int32_t* colorBits = new int32_t[nBits];
    GetDIBits(dc, iconInfo.hbmColor, 0, bm.bmHeight, colorBits, &bmi, DIB_RGB_COLORS);

    // Check whether the color bitmap has an alpha channel.
        // (On my Windows 7, all file icons I tried have an alpha channel.)
    BOOL hasAlpha = FALSE;
    for (int i = 0; i < nBits; i++) {
        if ((colorBits[i] & 0xff000000) != 0) {
            hasAlpha = TRUE;
            break;
        }
    }

    // If no alpha values available, apply the mask bitmap
    if (!hasAlpha) {
        // Extract the mask bitmap
        int32_t* maskBits = new int32_t[nBits];
        GetDIBits(dc, iconInfo.hbmMask, 0, bm.bmHeight, maskBits, &bmi, DIB_RGB_COLORS);
        // Copy the mask alphas into the color bits
        for (int i = 0; i < nBits; i++) {
            if (maskBits[i] == 0) {
                colorBits[i] |= 0xff000000;
            }
        }
        delete[] maskBits;
    }

    // Release DC and GDI bitmaps
    ReleaseDC(NULL, dc);
    ::DeleteObject(iconInfo.hbmColor);
    ::DeleteObject(iconInfo.hbmMask);

    // Create GDI+ Bitmap
    Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(bm.bmWidth, bm.bmHeight, bm.bmWidth * 4, PixelFormat32bppARGB, (BYTE*)colorBits);
    BITMAP_AND_BYTES ret = { bmp, colorBits };

    return ret;
}

static void saveFileIconAsPng(HICON hIcon, const std::wstring& pngFile) {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    BITMAP_AND_BYTES bbs = createAlphaChannelBitmapFromIcon(hIcon);
    IStream* fstrm = NULL;
    SHCreateStreamOnFile(pngFile.c_str(), STGM_WRITE | STGM_CREATE, &fstrm);
    CLSID pngClsid;
    /*
        bmp: {557cf400-1a04-11d3-9a73-0000f81ef32e}
        jpg: {557cf401-1a04-11d3-9a73-0000f81ef32e}
        gif: {557cf402-1a04-11d3-9a73-0000f81ef32e}
        tif: {557cf405-1a04-11d3-9a73-0000f81ef32e}
        png: {557cf406-1a04-11d3-9a73-0000f81ef32e}
    */
    CLSIDFromString(L"{557CF406-1A04-11D3-9A73-0000F81EF32E}", &pngClsid);
    bbs.bmp->Save(fstrm, &pngClsid, NULL);
    fstrm->Release();

    //delete bbs.bmp;
    //delete[] bbs.bytes;
    //DestroyIcon(hIcon);
}

HCURSOR mSetCursor(HCURSOR hCursor) {
    std::string filename = std::tmpnam(nullptr);
    //std::string filepath = "C:\\Users\\Vivien\\Downloads\\poop\\" + filename + ".ico";
    std::wstring widestr = std::wstring(filename.begin(), filename.end());
    //if (GetAsyncKeyState(VK_INSERT)) {
        //MessageBoxA(NULL, "Saving Icon", "Something", NULL);
        //SaveIcon(hCursor, widestr.c_str());
        saveFileIconAsPng(hCursor, widestr.c_str());
    //}
    return SetCursor(hCursor);
}