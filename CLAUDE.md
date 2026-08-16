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
- `external/DirectXMath/` — 공식 Microsoft DirectXMath 헤더 벤더링 (mingw 빌드 전용. mingw-w64 내장 `directxmath.h`는 `XMMATRIX` 없는 스텁이라 대체)

### Framework 폴더 분류

새 클래스는 아래 기준으로 배치한다. 각 폴더의 `README.md`에 담당 범위와 예정 클래스가 적혀 있다.

| 폴더 | 담당 | 현재 |
|---|---|---|
| `Framework/Core/` | D3D를 모르는 층 — 창·메시지 루프·입력·앱 골격 | `SystemClass`(튜토리얼 2), `ApplicationClass`, `InputClass`, `Check.h` |
| `Framework/Graphics/` | 렌더링 장치·데이터·상태 | `D3DClass`(튜토리얼 3: 디바이스/스왑체인/깊이버퍼/래스터라이저/행렬) |
| `Framework/Shaders/` | HLSL을 감싸는 C++ 래퍼 클래스 (완주 시 20개 이상이라 Graphics와 분리) | 비어 있음 |
| `Framework/Text/` | 2D 스프라이트·폰트·텍스트 (튜토리얼 11~) | 비어 있음 |
| `Framework/Utility/` | Timer/Fps/Cpu/Position 등 보조 기능 (튜토리얼 12~) | 비어 있음 |
| `assets/shaders/` | HLSL 원본 `.vs`/`.ps` — 코드가 아니라 런타임 로드 대상 | 비어 있음 |

### include 규칙

`Framework/`가 include 디렉터리로 등록되어 있다(CMake: `target_include_directories`, vcxproj: `AdditionalIncludeDirectories`).
자기 자신의 헤더를 포함해 **항상 폴더 접두사를 붙인 경로**로 쓴다. 파일을 옮겨도 그 파일의 include는 깨지지 않는다.

```cpp
#include "Core/ApplicationClass.h"
#include "Graphics/D3DClass.h"
#include "Shaders/ColorShaderClass.h"
```

### 에셋 로드 경로

셰이더 등 런타임 로드 파일은 **프로젝트 루트 기준 상대 경로**로 연다: `L"assets/shaders/color.vs"`.
VS의 작업 디렉터리(`$(ProjectDir)`)와 `scripts/mac-run.sh`(루트로 `cd` 후 실행) 모두 cwd가 프로젝트 루트라
빌드 산출물 옆으로 복사하지 않아도 양쪽에서 동작한다.

## 크로스컴파일 호환성 규칙 (중요)

MSVC와 mingw-w64(GCC) 둘 다에서 컴파일되어야 한다:

- `#pragma comment(lib, ...)` 금지 → 라이브러리는 `CMakeLists.txt`의 `target_link_libraries`에 추가 (기존 것은 `#ifdef _MSC_VER` 가드 처리됨)
- `sprintf_s` 등 `_s` 계열 → `#ifdef _MSC_VER` 분기 후 `snprintf` 사용 (Check.h 참고)
- SAL 어노테이션(`_In_` 등)은 mingw에서도 정의되므로 사용 가능
- UNICODE/_UNICODE는 CMake에서 정의됨. 문자열은 `L""` + `WCHAR` 계열 유지
- 셰이더는 HLSL 파일 + `D3DCompileFromFile` 런타임 컴파일 (Wine의 d3dcompiler로 동작). fxc 사전 컴파일 의존 금지
- 소스는 **UTF-8 BOM**으로 저장한다. MSVC는 BOM이 없으면 한글 주석을 시스템 코드페이지(CP949)로 읽어 C4819 경고를 낸다.
  방어책으로 `/utf-8`을 vcxproj(`AdditionalOptions`)와 CMakeLists.txt(`target_compile_options`) 양쪽에 넣어뒀다. GCC(mingw)는 기본이 UTF-8이라 무관

## 다음 할 일

- macOS 세팅 **완료** (2026-08-12): GPTK(D3DMetal) 설치, 튜토리얼 3의 D3D11 초기화까지 macOS에서 실행 확인
- 폴더 구조 정리 **완료** (2026-08-16): `Framework/`를 Core/Graphics/Shaders/Text/Utility로 분할, `assets/` 신설
- 다음: 튜토리얼 4 (버퍼, 셰이더, 삼각형 렌더링)
  - `Framework/Graphics/` → `ModelClass`, `CameraClass`
  - `Framework/Shaders/` → `ColorShaderClass`
  - `assets/shaders/` → `color.vs`, `color.ps` (`D3DCompileFromFile` 런타임 컴파일 유지)
  - 새 소스는 vcxproj(+`.filters`)와 CMakeLists.txt **양쪽에** 등록할 것
- Rider: CMakePresets.json의 프리셋을 그대로 인식 (`macOS mingw (Debug)` 활성화, 기본 Debug 프로필은 끔)

## 배경 리서치 요약 (2026-08 조사)

- GPTK/D3DMetal, DXMT, DXVK 전부 Wine 기반 — macOS에서 D3D11의 "완전 네이티브" 실행은 불가
- dxvk-native는 macOS 미지원. DXVK 2.x는 MoltenVK가 요구 Vulkan 기능 미충족으로 macOS에서 1.10.3 고정
- D3D11 게임 실행 성능/호환성: DXMT ≥ D3DMetal > DXVK(macOS)
- 진짜 네이티브가 필요해지면: Metal 직접 포팅 or Diligent Engine(D3D11 유사 API) — 튜토리얼 완주 후 별도 프로젝트로 검토
