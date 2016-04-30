// spapirun.c - runs SPCHAPI.EXE with appropriate parameters (ignoring what's passed to this)
//              'WINTDIST.EXE' will be exec'd without a UI and will run in the background while
//              this application waits for it to complete.

#include "windows.h"

#pragma comment(lib,"shell32.lib")


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmd, int nCmdShow)
{
char tbuf[MAX_PATH];
char *p1;
DWORD dwExitCode;
SHELLEXECUTEINFO sei;


  if(!GetModuleFileName(NULL, tbuf, sizeof(tbuf)))
  {
    return(1001);
  }

  p1 = strrchr(tbuf,'\\');
  if(!p1)
    return(1002);

  p1[1] = 0;
  strcat(p1, "SPCHAPI.EXE");

  memset(&sei, 0, sizeof(sei));
  sei.cbSize = sizeof(sei);
  sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
  sei.lpFile = tbuf;
  sei.lpParameters = "/Q"; // "";
  sei.nShow = SW_SHOWNORMAL;

  if(!ShellExecuteEx(&sei))
  {
    return(1003);
  }
  else
  {
    WaitForSingleObject(sei.hProcess, INFINITE);

    if(!GetExitCodeProcess(sei.hProcess, &dwExitCode))
    {
      return(1004);
    }
    else if(dwExitCode == STILL_ACTIVE)
    {
      return(1005);
    }
    else if(dwExitCode)
    {
      return((int)dwExitCode);
    }
  }

  return(0);

}
