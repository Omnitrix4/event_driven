#!/bin/bash

# 查找当前目录及其子目录中的所有 .cpp 和 .h 文件，并使用 clang-format 进行格式化
find . -name "*.cpp" -o -name "*.h" | xargs clang-format -i
