# RastertekSample

Rastertek DirectX 11 튜토리얼 학습 프로젝트 (내일배움캠프 온보딩, 현재 튜토리얼 2 완료).

## 프로젝트 목표

- Windows(Win32 + D3D11) 코드로 Rastertek 튜토리얼을 진행한다
- **macOS에서도 개발/실행한다** — 단, Metal 포팅이 아니라 크로스컴파일 + Wine 방식:
  - mingw-w64로 Windows x64 `.exe` 크로스컴파일
  - Wine(wine-stable) + DXMT(또는 GPTK/D3DMetal)로 실행
- 코드는 100% Windows 코드 유지. 플랫폼 분기 없음. 브랜치는 `master` 하나.

## 빌드 시스템 (이원화)

| 환경 | 방법 |
|---|---|
| Windows | 기존 `RastertekSample.sln` (Visual Studio) — 그대로 유지, 건드리지 말 것 |
| macOS | `CMakeLists.txt` + `cmake/toolchain-mingw-x86_64.cmake` |

macOS 빌드/실행:

```sh
./scripts/mac-build.sh   # → build-mac/RastertekSample.exe
./scripts/mac-run.sh     # → WINEPREFIX=.wineprefix 로 wine 실행
```

사전 요구사항: `brew install cmake ninja mingw-w64` + `brew install --cask wine-stable`
(구 `gcenx/wine/wine-crossover` cask는 2026-04 삭제됨. `wine-stable`도 deprecated라 2026-09 이후엔
[Gcenx/macOS_Wine_builds](https://github.com/Gcenx/macOS_Wine_builds/releases)에서 직접 받는다.
Gatekeeper에 막히면 `xattr -dr com.apple.quarantine "/Applications/Wine Stable.app"`)
상세 가이드: `docs/MAC_SETUP.md`

## 코드 구조

- `main.cpp` — `WinMain` 진입점 (ANSI WinMain, 빌드 대상)
- `RastertekSample.cpp` — VS 템플릿 잔재. **빌드에서 제외됨**, 무시할 것
- `Framework/SystemClass` — 창 생성, 메시지 루프 (튜토리얼 2 완료 상태)
- `Framework/InputClass` — 키보드 입력
- `Framework/ApplicationClass` — 프레임 루프 (아직 D3DClass 미연결)
- `Framework/D3DClass` — D3D11 초기화 **작성 중** (Initialize가 미완성, 튜토리얼 3 진행 예정)
- `Framework/Check.h` — HRESULT/포인터/bool 공용 검사 매크로 (`CHECK_RETURN`)
- `external/DirectXMath/` — 공식 Microsoft DirectXMath 헤더 벤더링 (mingw 빌드 전용. mingw-w64 내장 `directxmath.h`는 `XMMATRIX` 없는 스텁이라 대체)

## 크로스컴파일 호환성 규칙 (중요)

MSVC와 mingw-w64(GCC) 둘 다에서 컴파일되어야 한다:

- `#pragma comment(lib, ...)` 금지 → 라이브러리는 `CMakeLists.txt`의 `target_link_libraries`에 추가 (기존 것은 `#ifdef _MSC_VER` 가드 처리됨)
- `sprintf_s` 등 `_s` 계열 → `#ifdef _MSC_VER` 분기 후 `snprintf` 사용 (Check.h 참고)
- SAL 어노테이션(`_In_` 등)은 mingw에서도 정의되므로 사용 가능
- UNICODE/_UNICODE는 CMake에서 정의됨. 문자열은 `L""` + `WCHAR` 계열 유지
- 셰이더는 HLSL 파일 + `D3DCompileFromFile` 런타임 컴파일 (Wine의 d3dcompiler로 동작). fxc 사전 컴파일 의존 금지

## 다음 할 일

macOS 첫 세팅은 **완료됨** (2026-08-12): wine-stable 11.0 설치, `mac-build.sh` 빌드 및
`mac-run.sh` 실행("Engine" 창 생성) 확인. gstreamer-runtime cask는 sudo가 필요해 건너뜀
(미디어 재생용이라 튜토리얼에는 불필요 — 필요해지면 `brew install --cask gstreamer-runtime`).

1. 튜토리얼 3 (D3D11 초기화) 진행 시:
   - `D3DClass::Initialize` 완성 (현재 미완성 — 반환문 없음)
   - D3D11 디바이스가 실제 생성되면 DXMT 설치 필요: https://github.com/3Shain/dxmt (wiki의 Installation Guide 참고. 최신 DXMT는 Wine 10.18+ 지원 — 현재 설치된 wine-stable 11.0이면 충분)
2. Rider 사용 시: 2026.1+ 필요, CMake options에 `-DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw-x86_64.cmake`

## 배경 리서치 요약 (2026-08 조사)

- GPTK/D3DMetal, DXMT, DXVK 전부 Wine 기반 — macOS에서 D3D11의 "완전 네이티브" 실행은 불가
- dxvk-native는 macOS 미지원. DXVK 2.x는 MoltenVK가 요구 Vulkan 기능 미충족으로 macOS에서 1.10.3 고정
- D3D11 게임 실행 성능/호환성: DXMT ≥ D3DMetal > DXVK(macOS)
- 진짜 네이티브가 필요해지면: Metal 직접 포팅 or Diligent Engine(D3D11 유사 API) — 튜토리얼 완주 후 별도 프로젝트로 검토
