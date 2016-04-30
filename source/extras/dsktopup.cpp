// dsktopup.cpp - desktop 'updater' - reads wallpaper info, "updates" desktop with it

#include "windows.h"
#include "shellapi.h"
#include "shlobj.h"

#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"shell32.lib")
#pragma comment(lib,"advapi32.lib")
#pragma comment(lib,"ole32.lib")

int main()
{
  char tbuf[MAX_PATH] = "";

  // TODO:  read each *NEW* setting and update the desktop accordingly

  HKEY hKey;
  if(RegOpenKeyEx(HKEY_CURRENT_USER, "Control Panel\\Desktop", 0, KEY_QUERY_VALUE,
                  &hKey) == ERROR_SUCCESS)
  {
    DWORD dwType, cbData = MAX_PATH;
    if(RegQueryValueEx(hKey, "Wallpaper", NULL, &dwType, (LPBYTE)tbuf, &cbData)
       != ERROR_SUCCESS)
    {
      tbuf[0] = 0;
    }
    else
    {
      tbuf[cbData] = 0;
    }

    RegCloseKey(hKey);
  }

  SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (LPVOID)tbuf, SPIF_SENDCHANGE);

  InvalidateRect(GetDesktopWindow(), NULL, TRUE);
  return(0);
}

