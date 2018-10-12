@echo off
if "%VULKAN_SDK%" == "" goto NOENVVAR

rem Compile all shaders
echo Compiling shaders
setlocal EnableDelayedExpansion
for %%f in (*.vert, *.frag) do (
    set x=%%~xf
    set name=%%~nf_!x:~1!.spv
    %VULKAN_SDK%\Bin\glslangValidator.exe -V %%f -o !name! -s
    set line="%%~nxf => !name!"
    echo %%~nxf ^=^> !name!
)
echo Shaders compiled
goto END

:NOENVVAR
echo The "VULKAN_SDK" environment variable is not set.
echo Please set the environment variable to point to your vulkan sdk folder!
goto END

:END
pause