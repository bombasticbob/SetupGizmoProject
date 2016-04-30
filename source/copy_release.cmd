@ECHO OFF

echo this copies release files to the 'images' directory
echo as well as appropriate executables to 'Debug' and 'Release' directories

echo .
echo copying files that might be expected to be in the same dir as the EXE
copy SETUP\Debug\Setup.exe Debug\
copy UNINST\Debug\Uninst.exe Debug\
copy AutoRun\Debug\AutoRun.exe Debug\
copy AutoRun\res\AutoRun.ico Debug\
copy res\autorun.bmp Debug\

copy SETUP\Release\Setup.exe Release\
copy UNINST\Release\Uninst.exe Release\
copy AutoRun\Release\AutoRun.exe Release\
copy AutoRun\res\AutoRun.ico Release\
copy res\autorun.bmp Release\

echo .
echo copying release files to IMAGES
copy Release\SetupGiz.exe IMAGES\SetupGiz.exe
copy Release\SelfExtractorMaker.exe IMAGES\SelfExtractorMaker.exe
copy SETUP\Release\Setup.exe IMAGES\Setup.exe
copy UNINST\Release\Uninst.exe IMAGES\Uninst.exe
copy AutoRun\Release\AutoRun.exe IMAGES\AutoRun.exe

echo .
echo additional resources that are used by CDROM etc.
copy AutoRun\res\AutoRun.ico IMAGES\AutoRun.ico
copy res\autorun.bmp IMAGES\autorun.bmp

echo .
echo help file
copy help\Output\SetupGiz.chm IMAGES\SetupGiz.chm

