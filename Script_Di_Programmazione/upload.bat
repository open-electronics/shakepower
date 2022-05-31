#
# https://www.horstmuc.de/wbat32.htm#inifile
#



cd /d %~dp0

cls

@echo off


echo.
echo    ##############################################################################
echo    #                          FUTURA ELETTRONICA                                #
echo    #                                                                            #
echo    #                        Programmazione Attiny85                             #
echo    #                                                                            #
echo    #                                                                            #
echo    ##############################################################################

echo.


echo Si prega di attendre, sto cercando le porte seriali.

FOR /F "delims=^T tokens=1,2" %%a IN ('powershell -command "Get-WMIObject Win32_SerialPort | Select-Object Description, @{Name = 'Device'; Expression = { '^' + $_.DeviceID }}  | Where {$_.Description -Match 'Programming Port'}"' ) DO (
    SET "CPNAME=%%a"
    SET "CPNUM=%%b"
)


echo.
echo Ho trovato le seguenti porte:
echo.

echo  CPNAME is: %CPNAME%
echo  CPNUM  is: %CPNUM%


echo.
set /p COM_PORT=Inserire in numero della porta da utilizzare COM:
echo.



CHOICE /C ABCDEN /N /T 10 /D A /M "Selezionare il BoudRate A:921600, B:115200, C:256000, D:230400 , E:512000 or Default?"
IF ERRORLEVEL 1 SET BAUD=921600
IF ERRORLEVEL 2 SET BAUD=115200
IF ERRORLEVEL 3 SET BAUD=256000
IF ERRORLEVEL 4 SET BAUD=230400
IF ERRORLEVEL 5 SET BAUD=512000
IF ERRORLEVEL 6 SET BAUD=921600

echo.

echo La porta   : COM%COM_PORT%
echo Il Boudrate: %BAUD% Bit/sec


echo.
@ECHO OFF
SET /P yesno=VUOI PROCEDERE A PROGRAMMARE IL Chip Attiny85? [Y/N]:
IF "%yesno%"=="y" GOTO Confirm
IF "%yesno%"=="Y" GOTO Confirm
GOTO Exit


:Confirm


:Begin


echo.
SLEEP 4



avrdude -u -c stk500 -p t85 -P COM%COM_PORT% -b %BAUD% -F -V -e -U flash:w:"TimerRettrigerable.ino.hex":a -U lfuse:w:0xE2:m -U hfuse:w:0xDC:m -U efuse:w:0xFF:m 



:End
ECHO.


:Exit
ECHO.
echo Bye Bye
SLEEP 3


