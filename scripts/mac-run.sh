#!/usr/bin/env bash
# 크로스컴파일된 exe를 Wine으로 실행
# 필요: docs/MAC_SETUP.md 참고 (wine-crossover 또는 Game Porting Toolkit)
set -euo pipefail
cd "$(dirname "$0")/.."

EXE=build-mac/RastertekSample.exe
[ -f "$EXE" ] || { echo "먼저 scripts/mac-build.sh 로 빌드하세요."; exit 1; }

# 프로젝트 전용 Wine 프리픽스 (시스템 오염 방지)
export WINEPREFIX="$PWD/.wineprefix"
export WINEDEBUG="${WINEDEBUG:--all}"

# Wine 바이너리 탐색: GPTK(gameportingtoolkit)가 있으면 우선, 없으면 wine
WINE_BIN="$(command -v wine64 || command -v wine || true)"
if [ -z "$WINE_BIN" ]; then
    echo "wine을 찾을 수 없습니다. docs/MAC_SETUP.md 참고."
    exit 1
fi

"$WINE_BIN" "$EXE"
