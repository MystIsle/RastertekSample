# macOS(또는 Linux)에서 Windows x86_64 PE 실행 파일을 크로스컴파일하기 위한 툴체인.
# 필요: brew install mingw-w64
#
# 사용법:
#   cmake -B build-mac -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw-x86_64.cmake -DCMAKE_BUILD_TYPE=Debug
#   cmake --build build-mac

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(TOOLCHAIN_TRIPLE x86_64-w64-mingw32)

find_program(CMAKE_C_COMPILER   ${TOOLCHAIN_TRIPLE}-gcc     REQUIRED)
find_program(CMAKE_CXX_COMPILER ${TOOLCHAIN_TRIPLE}-g++     REQUIRED)
find_program(CMAKE_RC_COMPILER  ${TOOLCHAIN_TRIPLE}-windres REQUIRED)

# 호스트(macOS) 라이브러리/헤더를 절대 찾지 않도록
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
