#!/usr/bin/env bash
# winedbg gdb 프록시로 소스 레벨 디버깅 서버를 띄운다 (배경: docs/MAC_DEBUGGING.md)
# 사용:
#   scripts/mac-debug.sh [exe경로]   # 기본 build-mac/RastertekSample.exe
# 이후 다른 터미널에서 안내되는 gdb 명령으로 접속한다.
set -euo pipefail
cd "$(dirname "$0")/.."

EXE="${1:-build-mac/RastertekSample.exe}"
PORT="${PORT:-2159}"
[ -f "$EXE" ] || { echo "$EXE 없음 — 먼저 scripts/mac-build.sh 로 빌드하세요."; exit 1; }

# winedbg가 포함된 Wine이 필요하다 (GPTK에는 winedbg가 없음).
# Gcenx/macOS_Wine_builds 릴리스의 wine-devel tarball을 /Applications에 풀면 자동 인식.
WINE_BIN="${WINE_DEBUG_WINE:-}"
if [ -z "$WINE_BIN" ]; then
    for cand in "/Applications/Wine Devel.app/Contents/Resources/wine/bin/wine" \
                "/Applications/Wine Stable.app/Contents/Resources/wine/bin/wine"; do
        [ -x "$cand" ] && WINE_BIN="$cand" && break
    done
fi
[ -n "$WINE_BIN" ] || { echo "winedbg 포함 Wine을 못 찾음. WINE_DEBUG_WINE=<wine경로> 로 지정하거나 docs/MAC_DEBUGGING.md 참고."; exit 1; }

# GPTK 프리픽스와 분리된 디버깅 전용 프리픽스
export WINEPREFIX="${WINEPREFIX:-$PWD/.wineprefix-debug}"
export WINEDEBUG="${WINEDEBUG:--all}"

# winedbg는 유닉스식 경로를 못 받는다 — Windows식(Z:\...)으로 변환
WINPATH="Z:$(cd "$(dirname "$EXE")" && pwd | tr '/' '\\')\\$(basename "$EXE")"

echo "== winedbg gdb 서버 시작 (port $PORT) — 다른 터미널에서:"
echo "   gdb $EXE"
echo "   (gdb) set architecture i386:x86-64"
echo "   (gdb) target remote localhost:$PORT"
echo "   (gdb) break D3DClass::Initialize   # 예시"
echo "   (gdb) continue"
echo
exec "$WINE_BIN" winedbg --gdb --no-start --port "$PORT" "$WINPATH"
