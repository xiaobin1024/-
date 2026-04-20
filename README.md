# 网络词典 (Network Dictionary)

## 功能特性

- 单词查询与翻译
- 收藏单词管理
- 生词本功能
- 导出收藏单词（Excel/PDF）

## 构建要求

- Qt 5/6 (Widgets, Network, TextToSpeech, PrintSupport)
- CMake &gt;= 3.16
- C++17
- vcpkg（用于 xlnt）

## 首次克隆

```bash
cd network_dictionarygit clone --recursive https://github.com/yourname/network_dictionary.git
```

## 安装依赖

```bash
cd src/vcpkg
.\vcpkg.exe install xlnt:x64-windows
```

## 构建

```base
mkdir build && cd build
cmake ..
cmake --build .
```

## 快速开始

### 方式一：手动安装

    git clone --recursive https://github.com/yourname/network_dictionary.git
    cd network_dictionary/src/vcpkg
    .\vcpkg.exe install xlnt:x64-windows
    cd ../..
    mkdir build && cd build
    cmake ..
    cmake --build .

### 方式二：双击setup.bat脚本文件


