# macOS에서 디버깅 — 딥리서치 결과 (2026-08)

전제: mingw-w64 크로스컴파일 PE(DWARF 심볼) + GPTK(D3DMetal) Wine 실행 환경.
결론 요약 먼저:

| 목적 | 방법 | 판정 |
|---|---|---|
| C++ 브레이크포인트 (가장 확실) | Windows 11 ARM VM + Visual Studio | ✅ 확실, 무료 가능 |
| C++ 브레이크포인트 (맥 네이티브) | winedbg --gdb + Homebrew gdb | ⚠️ 유력하나 macOS 실증 없음 |
| GPU/셰이더 디버깅 | GPTK Metal 프레임 캡처 + Xcode | ✅ Apple 공식 지원 |
| Rider에서 직접 | — | ❌ 불가 (Remote GDB 구성 없음) |
| lldb 직접 attach | — | ❌ 사실상 불가 |

## 1. 확실한 경로 — Windows VM (추천)

**VMware Fusion(2024년부터 완전 무료) + Windows 11 ARM + VS Community**면 비용 0으로
진짜 Visual Studio 브레이크포인트 디버깅이 된다.

- VS 2022 17.4+는 ARM64 네이티브. x64/ARM64 둘 다 빌드 가능
- 쾌적함은 `.sln`에 ARM64 구성을 추가해 VM 안에서 네이티브 디버깅하는 것 (이 코드베이스는 순수 Win32+D3D11이라 소스 수정 없이 컴파일됨). x64 exe도 Prism 에뮬레이션으로 디버깅 가능
- **mingw 빌드 exe는 VS에서 디버깅 불가** (DWARF ≠ PDB) — VM에서는 반드시 .sln 빌드 사용
- D3D11: Parallels(연 $99.99)가 DX11.1 공식 지원으로 품질 우위, Fusion은 무료지만 3D 가속 이슈 보고 있음. 먼저 Fusion으로 시작 → 문제 있으면 Parallels 체험판
- **RenderDoc**: Windows ARM64 네이티브 빌드가 없어서 ARM64 exe는 캡처 불가.
  **캡처하려면 x64 빌드 + x64 RenderDoc** (Parallels에서 동작 사례 있음, 품질 보장은 없음).
  PIX는 ARM64 있으나 GPU 캡처는 D3D12 전용(D3D11은 11On12 우회, VM 동작 미검증)
- 진지한 프레임 캡처가 필요하면: Azure GPU VM(NV12ads ~$0.9/시간) + Parsec이 시간제로 가장 저렴

## 2. 맥 네이티브 경로 — winedbg gdb 프록시 (실험 가치 있음)

구조: winedbg가 Wine 안에서 gdbserver 역할 → macOS의 gdb가 TCP로 접속.
클라이언트는 attach를 안 하므로 Rosetta/arm64 문제를 우회한다.

```sh
brew install gdb        # arm64 네이티브 + --enable-targets=all (PE 심볼 읽기 가능)

# 터미널 1: winedbg를 gdbserver로 (GPTK에는 winedbg가 없음 — 아래 참고)
wine winedbg --gdb --no-start --port 2159 build-mac/RastertekSample.exe

# 터미널 2: 호스트 gdb로 접속
gdb build-mac/RastertekSample.exe
(gdb) set architecture i386:x86-64
(gdb) target remote localhost:2159
```

- Linux에서는 검증된 표준 워크플로 (Binary Ninja 공식 문서화). **macOS 실증 사례는 못 찾음** — 되는지 실측 필요
- **GPTK에는 winedbg가 없다.** WineHQ 공식 macOS 빌드(wine-stable)를 별도 프리픽스로 설치해
  "로직 디버깅은 wine-stable, 렌더링 확인은 GPTK" 이원화가 필요 (D3D 초기화 전 로직은 wine-stable로 충분)
- 실패 시 대안: MSYS2의 `gdbserver.exe`(Windows용 gdb 패키지에 포함)를 Wine 안에서 실행 → 같은 방식 접속
- 되더라도 한계: 콜스택 절단, 스테핑 불안정 (Linux에서도 보고됨). 로그 병행이 현실적
- IDE 연결: **Rider는 Remote GDB 구성이 없어 불가.** 되면 VS Code cppdbg(`miDebuggerServerAddress`)나
  CLion Remote Debug로 프런트엔드를 얹을 수 있음

## 3. GPU/셰이더 디버깅 — GPTK Metal 캡처 (의외의 수확)

Apple이 평가 환경에서 **공식 지원**하는 유일한 디버깅이 바로 이것:

```sh
MTL_HUD_ENABLED=1 ./scripts/mac-run.sh        # FPS/프레임타임 HUD
MTL_CAPTURE_ENABLED=1 ./scripts/mac-run.sh    # F10으로 GPU 프레임 캡처 → Xcode로 열기
```

- 캡처 파일을 Xcode Metal Debugger로 열면 드로우콜/리소스/셰이더(HLSL이 변환된 Metal 코드) 검사 가능
- `MTL_SHADER_VALIDATION=1`, `MTL_DEBUG_LAYER=1`도 사용 가능
- D3D11 관점이 아니라 Metal 관점이지만, "삼각형이 왜 안 나오지" 수준의 원인 추적에는 충분히 유용

## 4. 결론 (이 프로젝트 기준)

1. 평소: 로그 디버깅 (`Check.h` + `WINEDEBUG=+d3d11,+dxgi`) — 튜토리얼 수준에는 충분
2. 화면이 이상할 때: **GPTK Metal 캡처 + Xcode** (맥에서 바로 됨)
3. 브레이크포인트가 꼭 필요할 때: **VMware Fusion 무료 VM + VS Community** 세팅 (반나절 투자)
4. 궁금하면 실험: winedbg --gdb 경로 (성공하면 맥에서 CLI/VSCode 브레이크포인트 확보)

상세 근거·출처는 리서치 로그 참고: Parallels KB 129497, RenderDoc GitHub #2949/#3506,
Binary Ninja Wine debugging 문서, apple1417.dev "Debugging under Proton",
Homebrew gdb formula, Apple GPTK 공식 페이지 / WWDC24 세션 10089.
