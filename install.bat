echo off
REM run this batch file to copy the example in <XSI_HOME>\Application\Plugins folder.

set MODULE=XSIMan.dll
set STARTUP=%XSI_USERHOME%\Application\Plugins\XSIMan


if not exist "%STARTUP%" mkdir %STARTUP%
copy %MODULE% %STARTUP%


