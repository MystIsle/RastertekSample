cmake_minimum_required(VERSION 3.21)
project(RastertekSample CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows 전용 코드베이스.
# - Windows에서: MSVC 또는 Rider(CMake)로 그대로 빌드
# - macOS에서:   cmake/toolchain-mingw-x86_64.cmake 로 크로스컴파일 후 Wine으로 실행
if(NOT WIN32)
    message(FATAL_ERROR
        "이 프로젝트는 Windows PE 바이너리만 생성합니다.\n"
        "macOS에서는 mingw 툴체인으로 구성하세요:\n"
        "  cmake -B build-mac -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw-x86_64.cmake\n"
        "또는 scripts/mac-build.sh 를 사용하세요.")
endif()

add_executable(RastertekSample WIN32
    main.cpp
    Framework/Core/SystemClass.cpp
    Framework/Core/InputClass.cpp
    Framework/Core/ApplicationClass.cpp
    Framework/Graphics/D3DClass.cpp
    # 헤더 (IDE 표시용)
    Framework/Core/SystemClass.h
    Framework/Core/InputClass.h
    Framework/Core/ApplicationClass.h
    Framework/Core/Check.h
    Framework/Graphics/D3DClass.h
)

# 하위 폴더에 관계없이 항상 "Core/...", "Graphics/...", "Shaders/..." 로 include 한다
target_include_directories(RastertekSample PRIVATE Framework)

target_compile_definitions(RastertekSample PRIVATE
    UNICODE _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

# D3DClass.h의 #pragma comment(lib,...)는 MSVC 전용이므로 여기서 항상 명시적으로 링크
target_link_libraries(RastertekSample PRIVATE d3d11 dxgi d3dcompiler)

if(MINGW)
    # Wine 프리픽스에 mingw 런타임 DLL을 복사하지 않아도 되도록 정적 링크
    target_link_options(RastertekSample PRIVATE -static -static-libgcc -static-libstdc++)
    # mingw-w64의 directxmath.h는 타입 일부만 있는 스텁(XMMATRIX/수학 함수 없음).
    # 공식 Microsoft DirectXMath(헤더 온리, external/에 벤더링)를 우선 사용한다.
    target_include_directories(RastertekSample SYSTEM BEFORE PRIVATE external/DirectXMath)
endif()

if(MSVC)
    target_compile_options(RastertekSample PRIVATE /W4 /utf-8)
else()
    target_compile_options(RastertekSample PRIVATE -Wall)
endif()
