@echo off
chcp 65001 >nul
echo ========================================
echo   网络词典 - 环境初始化脚本
echo ========================================
echo.

:: 检查是否在项目根目录
if not exist "src\vcpkg" (
    echo [错误] 请在项目根目录运行此脚本
    pause
    exit /b 1
)

:: 1. 初始化 vcpkg 子模块
echo [1/3] 初始化 vcpkg 子模块...
git submodule update --init --recursive
if %errorlevel% neq 0 (
    echo [错误] 子模块初始化失败
    pause
    exit /b 1
)
echo [完成] 子模块已就绪
echo.

:: 2. 安装 xlnt 依赖
echo [2/3] 安装 xlnt 库（首次编译较慢，请等待）...
cd src\vcpkg
.\vcpkg.exe install xlnt:x64-windows
if %errorlevel% neq 0 (
    echo [错误] xlnt 安装失败
    pause
    exit /b 1
)
echo [完成] xlnt 已安装
echo.

:: 3. 创建构建目录
echo [3/3] 创建构建目录...
cd ..\..
if not exist "build" mkdir build
echo [完成] 构建目录已创建
echo.

echo ========================================
echo   初始化完成！
echo ========================================
echo.
echo 接下来请执行：
echo   cd build
echo   cmake ..
echo   cmake --build .
echo.
pause