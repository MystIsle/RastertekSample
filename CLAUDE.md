# RastertekSample

Rastertek DirectX 11 튜토리얼 학습 프로젝트 (내일배움캠프 온보딩, 현재 튜토리얼 3 완료).

## 프로젝트 목표

- Windows(Win32 + D3D11) 코드로 Rastertek 튜토리얼을 진행한다
- **macOS에서도 개발/실행한다** — 단, Metal 포팅이 아니라 크로스컴파일 + Wine 방식:
  - mingw-w64로 Windows x64 `.exe` 크로스컴파일
  - Wine(GPTK, D3DMetal 내장)로 실행
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

사전 요구사항: `brew install cmake ninja mingw-w64` + `brew trust gcenx/wine && brew install --cask game-porting-toolkit`
(D3DMetal 내장이라 D3D11이 바로 동작. wine-stable/DXMT/wine-crossover를 쓰지 않는 이유는 `docs/MAC_SETUP.md` 참고)
상세 가이드: `docs/MAC_SETUP.md`

## 코드 구조

- `main.cpp` — `WinMain` 진입점 (ANSI WinMain, 빌드 대상)
- `RastertekSample.cpp` — VS 템플릿 잔재. **빌드에서 제외됨**, 무시할 것
- `Framework/SystemClass` — 창 생성, 메시지 루프 (튜토리얼 2 완료 상태)
- `Framework/InputClass` — 키보드 입력
- `Framework/ApplicationClass` — 프레임 루프 (아직 D3DClass 미연결)
- `Framework/D3DClass` — D3D11 초기화 (튜토리얼 3 완료: 디바이스/스왑체인/깊이버퍼/래스터라이저/행렬)
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

- macOS 세팅 **완료** (2026-08-12): GPTK(D3DMetal) 설치, 튜토리얼 3의 D3D11 초기화까지 macOS에서 실행 확인
- 다음: 튜토리얼 4 (버퍼, 셰이더, 삼각형 렌더링) — ColorShaderClass/ModelClass/CameraClass 추가.
  새 소스는 vcxproj와 CMakeLists.txt **양쪽에** 등록할 것. HLSL은 `D3DCompileFromFile` 런타임 컴파일 유지
- Rider: CMakePresets.json의 프리셋을 그대로 인식 (`macOS mingw (Debug)` 활성화, 기본 Debug 프로필은 끔)

## 배경 리서치 요약 (2026-08 조사)

- GPTK/D3DMetal, DXMT, DXVK 전부 Wine 기반 — macOS에서 D3D11의 "완전 네이티브" 실행은 불가
- dxvk-native는 macOS 미지원. DXVK 2.x는 MoltenVK가 요구 Vulkan 기능 미충족으로 macOS에서 1.10.3 고정
- D3D11 게임 실행 성능/호환성: DXMT ≥ D3DMetal > DXVK(macOS)
- 진짜 네이티브가 필요해지면: Metal 직접 포팅 or Diligent Engine(D3D11 유사 API) — 튜토리얼 완주 후 별도 프로젝트로 검토
