# RastertekSample

[Rastertek DirectX 11 튜토리얼](https://rastertek.com/tutdx11win10.html) 학습 프로젝트.

코드는 100% Windows(Win32 + D3D11) 코드로 유지하면서, macOS에서는 **mingw-w64 크로스컴파일 + Wine**으로 빌드/실행한다. Metal 포팅이 아니므로 플랫폼 분기 코드가 없고 브랜치도 `master` 하나다.

## 진행 상황

- ✅ Tutorial 2 — 프레임워크와 창 생성
- ✅ Tutorial 3 — DirectX 11 초기화 (macOS에선 GPTK/D3DMetal로 실행 확인)

## 빌드

| 환경 | 방법 |
|---|---|
| Windows | `RastertekSample.sln` (Visual Studio) |
| macOS | `./scripts/mac-build.sh` → `./scripts/mac-run.sh` (Wine 실행) |

macOS 상세 세팅(요구 도구, Wine, Rider 설정)은 [docs/MAC_SETUP.md](docs/MAC_SETUP.md) 참고.
CMake 프리셋(`macOS mingw (Debug)`)이 있어 Rider/CLion에서도 폴더만 열면 된다.

## 구조

```
main.cpp                  WinMain 진입점
Framework/
  SystemClass             창 생성, 메시지 루프
  InputClass              키보드 입력
  ApplicationClass        프레임 루프
  D3DClass                D3D11 초기화
  Check.h                 HRESULT/포인터 검사 매크로
external/DirectXMath/     MS 공식 DirectXMath (mingw 빌드용 벤더링)
cmake/, scripts/          macOS 크로스컴파일 툴체인/스크립트
```

## 새 소스 파일 추가 시

빌드 시스템이 이원화되어 있으므로 **두 곳 모두** 등록해야 한다:

1. Visual Studio: 프로젝트에 파일 추가 (`.vcxproj`에 반영됨)
2. `CMakeLists.txt`: `add_executable(...)` 소스 목록에 추가
