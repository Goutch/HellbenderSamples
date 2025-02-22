@echo off
setlocal

:: Set the submodule path (Change this to your actual submodule path)
set SUBMODULE_PATH=HBE/


:: Check if a commit message is provided
if "%~1"=="" (
    echo Usage: commit_submodule.bat "commit message"
    exit /b 1
)

set COMMIT_MESSAGE=%~1

:: Check if the submodule exists
if not exist "%SUBMODULE_PATH%\.git" (
    echo Error: %SUBMODULE_PATH% is not a valid Git submodule.
    exit /b 1
)




:: Change to the submodule directory
cd /d "%SUBMODULE_PATH%" || exit /b 1

:: Ensure we are on the master branch
git checkout master 2>nul || git checkout -b master origin/master

:: Add and commit changes
git add .
git commit -m "%COMMIT_MESSAGE%"

:: Push changes
git push origin master
if %errorlevel% neq 0 (
    echo Failed to push submodule changes.
    exit /b 1
)

:: Move back to the main repository
cd /d ..

:: Stage and commit the updated submodule reference
git add "%SUBMODULE_PATH%"
git commit -m "Updated submodule reference: %COMMIT_MESSAGE%"
git push origin master

echo Submodule updated successfully!
exit /b 0