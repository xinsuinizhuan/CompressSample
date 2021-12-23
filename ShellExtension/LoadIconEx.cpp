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
#include "LoadIconEx.h"
#include <CommCtrl.h>

#pragma comment(lib, "Comctl32.lib")
#pragma comment(linker, "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HICON LoadIconEx(HINSTANCE hInstance, LPCWSTR lpIconName)
{
	HICON hIcon = nullptr;
	if (FAILED(LoadIconWithScaleDown(hInstance, lpIconName, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), &hIcon)))
	{
		// fallback, just in case
		hIcon = LoadIcon(hInstance, lpIconName);
	}
	return hIcon;
}

HICON LoadIconEx(HINSTANCE hInstance, LPCWSTR lpIconName, int iconWidth, int iconHeight)
{
	// the docs for LoadIconWithScaleDown don't mention that a size of 0 will
	// use the default system icon size like for e.g. LoadImage or LoadIcon.
	// So we don't assume that this works but do it ourselves.
	if (iconWidth == 0)
		iconWidth = GetSystemMetrics(SM_CXSMICON);
	if (iconHeight == 0)
		iconHeight = GetSystemMetrics(SM_CYSMICON);
	HICON hIcon = nullptr;
	if (FAILED(LoadIconWithScaleDown(hInstance, lpIconName, iconWidth, iconHeight, &hIcon)))
	{
		// fallback, just in case
		hIcon = static_cast<HICON>(LoadImage(hInstance, lpIconName, IMAGE_ICON, iconWidth, iconHeight, LR_DEFAULTCOLOR));
	}
	return hIcon;
}
