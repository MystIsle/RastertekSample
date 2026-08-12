#!/usr/bin/env bash
# 크로스컴파일된 exe를 Wine으로 실행
# 필요: docs/MAC_SETUP.md 참고 (wine-crossover 또는 Game Porting Toolkit)
set -euo pipefail
cd "$(dirname "$0")/.."

# 인자 없으면 Debug(build-mac), "Release" 넘기면 build-mac-release, 경로를 직접 넘겨도 된다
case "${1:-}" in
    "")        EXE=build-mac/RastertekSample.exe ;;
    Release)   EXE=build-mac-release/RastertekSample.exe ;;
    *)         EXE="$1" ;;
esac
[ -f "$EXE" ] || { echo "$EXE 없음 — 먼저 scripts/mac-build.sh 로 빌드하세요."; exit 1; }

# 프로젝트 전용 Wine 프리픽스 (시스템 오염 방지).
# 리포지토리 밖에 두는 이유: 프리픽스 내부의 심볼릭 링크(dosdevices/z: → / 등) 때문에
# IDE 인덱서가 디스크 전체를 크롤링하는 사고 방지 (CLion에서 실제 발생).
export WINEPREFIX="${WINEPREFIX:-$HOME/Library/Caches/RastertekSample/wineprefix}"
mkdir -p "$WINEPREFIX"
export WINEDEBUG="${WINEDEBUG:--all}"

# Wine 바이너리 탐색: GPTK(gameportingtoolkit)가 있으면 우선, 없으면 wine
WINE_BIN="$(command -v wine64 || command -v wine || true)"
if [ -z "$WINE_BIN" ]; then
    echo "wine을 찾을 수 없습니다. docs/MAC_SETUP.md 참고."
    exit 1
fi

"$WINE_BIN" "$EXE"
