set BRANCH= %~1

git submodule set-branch --branch %BRANCH% HBE

git submodule sync
git add .gitmodules
git commit -m "Update submodule to track %BRANCH% branch"