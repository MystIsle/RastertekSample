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

## ✨ 이 리포지토리의 특이점: 맥에서 DirectX 11을 "풀 워크플로"로

플랫폼 분기 코드 0줄로, macOS(Apple Silicon)에서 아래가 전부 동작한다:

```
내 C++ 코드 (100% Win32 + D3D11)
  → mingw-w64 크로스컴파일 (Windows x64 PE)
  → Rosetta 2 (x64 → arm64)
  → Wine / Apple GPTK (Win32 API)
  → D3DMetal (D3D11 → Metal)
  → Apple GPU 🟩
```

| 하고 싶은 것 | 방법 | 상태 |
|---|---|---|
| 빌드+실행 (렌더링 포함) | Rider/CLion ▶ `RastertekSample (Wine)` 또는 `mac-run.sh` | ✅ |
| **C++ 브레이크포인트 — D3D 살아있는 채로** | `mac-debug.sh` (GPTK 안에서 gdbserver.exe) + CLion GUI | ✅ 렌더 루프 매 프레임 정지 |
| GPU 프레임 캡처/셰이더 분석 | `MTL_CAPTURE_ENABLED=1` + Xcode Metal Debugger | ✅ Apple 공식 |
| Windows 교차 검증 | 같은 코드를 VS(.sln)로 | ✅ 이원화 빌드 |

핵심 트릭 (자세한 과정: [docs/MAC_DEBUGGING.md](docs/MAC_DEBUGGING.md)):

- **디버거를 앱 쪽으로 데려오기** — winedbg는 Wine에 종속이라 D3DMetal 없는 빌드에서만 가능했지만,
  MSYS2의 `gdbserver.exe`는 순수 Windows 프로그램이라 **GPTK 안에서** 실행 가능.
  덕분에 D3DMetal이 살아있는 채로(창이 그려지는 채로) gdb 원격 프로토콜 디버깅이 성립한다
- 호스트 gdb(Homebrew, `--enable-targets=all`)는 TCP 클라이언트일 뿐이라 Rosetta/arm64 제약을 전부 우회
- CLion의 Remote Debug 구성(`.run/`)으로 GUI 브레이크포인트·변수 검사·콜스택까지 —
  이 조합(macOS + Wine gdb 원격 + CLion)은 조사 시점 기준 공개된 선례를 찾지 못했다

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
