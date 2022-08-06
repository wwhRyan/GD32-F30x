@echo off

@echo.>.\log.txt
@echo ----------------------------------------------------------------------- >>.\log.txt
@echo ----------------------------------------------------------------------- >>.\log.txt
@echo ----------------------------------------------------------------------- >>.\log.txt

:: @echo checking mechine type

:: if %1 == "" goto ERROR

:: @echo %1

@echo #    #   ##   #    # ###### 
@echo ##  ##  #  #  #   #  #      
@echo # ## # #    # ####   #####  
@echo #    # ###### #  #   #      
@echo #    # #    # #   #  #      
@echo #    # #    # #    # ###### 
@echo ...

:: -t"%1"
D:\InstalledSoftware\IDE\Keil_v5\UV4\UV4.exe -b -j0 -t"AthenaRaontechLcos" .\Template\Keil_project\Project.uvprojx -o .\..\..\log.txt
if not %ERRORLEVEL% == 0 goto ERROR

@REM if not %ERRORLEVEL% == 0 goto ERROR

@echo #####   ####  #    # #    # #       ####    ##   #####  
@echo #    # #    # #    # ##   # #      #    #  #  #  #    # 
@echo #    # #    # #    # # #  # #      #    # #    # #    # 
@echo #    # #    # # ## # #  # # #      #    # ###### #    # 
@echo #    # #    # ##  ## #   ## #      #    # #    # #    # 
@echo #####   ####  #    # #    # ######  ####  #    # #####  
@echo ...

D:\InstalledSoftware\IDE\Keil_v5\UV4\UV4.exe -f -j0 -t"AthenaRaontechLcos" .\Template\Keil_project\Project.uvprojx -o .\..\..\log.txt

if %ERRORLEVEL% == 0 goto SUCCESS
if not %ERRORLEVEL% == 0 goto ERROR

:SUCCESS
@echo    _____ __  __ _____ _____ ___   _____ _____
@echo   / ___// / / // ___// ___// _ \ / ___// ___/
@echo  (__  )/ /_/ // /__ / /__ /  __/(__  )(__  ) 
@echo /____/ \__,_/ \___/ \___/ \___//____//____/  
exit 0                                     

:ERROR
@echo   ___   _____ _____ ____   _____
@echo  / _ \ / ___// ___// __ \ / ___/
@echo /  __// /   / /   / /_/ // /    
@echo \___//_/   /_/    \____//_/     
exit -1