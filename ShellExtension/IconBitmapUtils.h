/*
***************************************************************************************************
* 许可声明：
* 本软件（含源码、二进制）为开源软件，遵循MIT开源许可协议（若以下声明与MIT许可协议发送冲突，以本声明为准）。
* 任何个人或组织都可以不受限制的使用、修改该软件。
* 任何个人或组织都可以以源代码和二进制的形式重新分发、使用该软件。
* 任何个人或组织都可以不受限制地将该软件（或修改后的任意版本）用于商业、非商业的用途。
* 
* 免责声明：
* 作者不对使用、修改、分发（以及其他任何形式的使用）该软件（及其他修改后的任意版本）所产生的后果负有任何法律责任。
* 作者也不对该软件的安全性、稳定性做出任何保证。
* 
* 使用、修改、分发该软件时需要保留上述声明，且默认已经同意上述声明。
***************************************************************************************************
*/
#pragma once
#include <windows.h>
#include <Uxtheme.h>
#pragma warning(push)
#pragma warning(disable : 4458)
#include <GdiPlus.h>
#pragma warning(pop)
#include <map>

typedef HRESULT(WINAPI* FN_GetBufferedPaintBits)(HPAINTBUFFER hBufferedPaint, RGBQUAD** ppbBuffer, int* pcxRow);
typedef HPAINTBUFFER(WINAPI* FN_BeginBufferedPaint)(HDC hdcTarget, const RECT* prcTarget, BP_BUFFERFORMAT dwFormat, BP_PAINTPARAMS* pPaintParams, HDC* phdc);
typedef HRESULT(WINAPI* FN_EndBufferedPaint)(HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget);

/**
 * \ingroup utils
 * provides helper functions for converting icons to bitmaps
 */
class IconBitmapUtils {
   public:
    IconBitmapUtils();
    ~IconBitmapUtils();

    HBITMAP IconToBitmap(HINSTANCE hInst, UINT uIcon);
    HBITMAP IconToBitmapPARGB32(HICON hIcon, int width, int height);
    HBITMAP IconToBitmapPARGB32(HINSTANCE hInst, UINT uIcon);
    HRESULT Create32BitHBITMAP(HDC hdc, const SIZE* psize, __deref_opt_out void** ppvBits, __out HBITMAP* phBmp) const;
    HRESULT ConvertBufferToPARGB32(HPAINTBUFFER hPaintBuffer, HDC hdc, HICON hicon, SIZE& sizIcon);
    bool HasAlpha(__in Gdiplus::ARGB* pargb, const SIZE& sizImage, int cxRow) const;
    HRESULT ConvertToPARGB32(HDC hdc, __inout Gdiplus::ARGB* pargb, HBITMAP hbmp, const SIZE& sizImage, int cxRow) const;

   private:
    std::map<UINT, HBITMAP> bitmaps;
};
