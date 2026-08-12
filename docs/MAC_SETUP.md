# macOS에서 Rastertek(D3D11) 개발/실행 가이드

## 전략 요약

코드는 **100% Windows(Win32 + D3D11) 코드 그대로** 유지한다. 맥에서는:

1. **mingw-w64**로 Windows `.exe`를 크로스컴파일하고
2. **Wine + D3D11→Metal 변환 레이어**(D3DMetal 또는 DXMT)로 실행한다

즉 "포팅"이 아니라 "맥을 Windows 빌드/실행 머신처럼 쓰는" 방식이다.
플랫폼 분기 코드가 없으므로 브랜치도 `master` 하나, 폴더 구조도 그대로다.

| 구성 요소 | 역할 |
|---|---|
| CMakeLists.txt | 공용 빌드 정의 (Windows에선 MSVC, 맥에선 mingw) |
| cmake/toolchain-mingw-x86_64.cmake | 맥 → Windows x64 크로스컴파일 툴체인 |
| scripts/mac-build.sh | 빌드 (`build-mac/RastertekSample.exe` 생성) |
| scripts/mac-run.sh | Wine으로 실행 (프리픽스는 `.wineprefix/`) |
| 기존 .sln/.vcxproj | Windows Visual Studio용으로 그대로 유지 |

> 참고: Wine은 에뮬레이터가 아니라 Win32 API 호환 레이어다. 다만 x64 exe이므로
> Apple Silicon에서는 CPU 명령만 Rosetta 2가 변환한다.

## 1. 빌드 도구 설치

```sh
brew install cmake ninja mingw-w64
```

확인: `x86_64-w64-mingw32-g++ --version`

## 2. Wine 설치 — Game Porting Toolkit (2026-08 기준 확정 경로)

```sh
brew trust gcenx/wine          # Homebrew 5의 서드파티 탭 신뢰 절차
brew install --cask game-porting-toolkit
```

Gcenx가 패키징한 Apple GPTK 3.0. **D3DMetal(D3D11/12→Metal)이 내장**되어 있어서 별도
변환 레이어 없이 D3D11 디바이스가 바로 생성된다 (튜토리얼 3의 `D3D11CreateDeviceAndSwapChain`
성공 확인). cask postflight가 quarantine 제거/재서명까지 처리하므로 xattr 수동 조치 불필요.
설치되는 명령은 `wine64` (`wine` 아님 — `scripts/mac-run.sh`가 알아서 찾는다).

### 다른 경로들을 쓰지 않는 이유 (2026-08 시점 확인 결과)

- **wine-stable (공식 WineHQ 11.0)**: 창 띄우기(튜토리얼 2)까지는 되지만 macOS에선
  wined3d가 GL 한계로 **FL 11_0 디바이스 생성 실패**. cask 자체도 Gatekeeper 문제로
  deprecated (2026-09-01 비활성화 예고).
- **DXMT**: winemac.drv에 Metal 뷰 심볼을 노출하는 **CrossOver 24+ 계열 Wine 전용**.
  공식 Wine 11.0에 DXMT v0.80을 설치하면 로드는 되지만
  "Failed to create metal view … no exported symbols" 로 실패한다.
  호환 Wine의 무료 배포(Gcenx winecx)는 중단됨 — 유료 CrossOver를 쓰지 않는 한 현재로선 GPTK가 답.
- **wine-crossover cask**: 2026-04 탭에서 삭제 (Wine 8.0.1 기반 구버전).

## 3. 빌드 & 실행

```sh
./scripts/mac-build.sh          # → build-mac/RastertekSample.exe
./scripts/mac-run.sh            # → Wine으로 실행
```

## 4. Rider에서 열기

- **Rider 2026.1+** 부터 CMake 기반 C++ 프로젝트를 Beta 지원한다 (그 이전 버전은 macOS에서 Unreal 프로젝트만 지원).
- **`.sln`이 아니라 프로젝트 폴더를 열 것** (sln은 MSVC 전용이라 macOS에서 빌드 불가).
- `CMakePresets.json`에 mingw 툴체인이 지정된 `macOS mingw (Debug/Release)` 프리셋이 있다.
  Rider가 프리셋을 자동 인식하므로 **Settings → Build, Execution, Deployment → CMake**에서
  프리셋 기반 프로필을 활성화하고 기본 Debug 프로필(툴체인 없음 → 가드 에러 발생)은 꺼둔다.
  빌드 디렉터리는 `build-mac`으로 `mac-build.sh`와 공유된다.
- 실행: `.run/` 폴더에 **"Wine 빌드+실행"** Shell Script 런 구성이 들어 있다 (Rider 자동 인식,
  `mac-build.sh && mac-run.sh` 수행). 실행 드롭다운에서 선택 후 ▶ 누르면 된다.
- 디버깅: Rider의 LLDB로는 Wine 위의 Windows exe를 소스 레벨 디버깅할 수 없다.
  printf/`OutputDebugString`(`WINEDEBUG=+debugstr`로 표시) 위주로 하고, 본격 디버깅은 Windows에서.
- Beta 지원이 불안정하면 CLion(비상업용 무료)이 대안이다. 같은 CMakeLists를 그대로 연다.

## 5. 코딩 시 주의사항 (MSVC ↔ mingw 차이)

- `#pragma comment(lib, ...)` 은 MSVC 전용 → 새 라이브러리는 CMakeLists.txt `target_link_libraries`에 추가
- mingw-w64의 `directxmath.h`는 타입 일부만 있는 스텁이라 `XMMATRIX`/수학 함수가 없다
  → 공식 [Microsoft DirectXMath](https://github.com/microsoft/DirectXMath) 헤더를 `external/DirectXMath/`에
  벤더링해두었고, mingw 빌드에서만 include 경로가 우선 적용된다 (Windows에선 SDK 것을 그대로 사용)
- `sprintf_s` 등 `_s` 계열 함수는 `#ifdef _MSC_VER` 분기 (Check.h 참고)
- 셰이더: `D3DCompileFromFile`(HLSL 런타임 컴파일)은 Wine의 d3dcompiler로 동작한다. 튜토리얼 진행에 문제 없음
- 디버깅: 맥에서는 printf/OutputDebugString 수준 (`WINEDEBUG` 환경변수 활용).
  브레이크포인트/그래픽 디버깅 선택지는 `docs/MAC_DEBUGGING.md` 참고 (GPTK Metal 캡처, VM, winedbg 등)

## 한계 (알고 시작하기)

- 이 방식은 "실행 확인"까지는 훌륭하지만 **네이티브 macOS 앱이 되는 것은 아니다**
- Wine/DXMT 버그와 실제 코드 버그를 구분해야 할 때가 가끔 있다 — 이상하면 Windows에서 교차 확인
- 진짜 네이티브(Metal) 포팅은 나중에 튜토리얼 이해도가 쌓인 뒤 별도 프로젝트로 하는 걸 추천
