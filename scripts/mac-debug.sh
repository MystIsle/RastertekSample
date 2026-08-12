#!/usr/bin/env bash
# 소스 레벨 디버깅 서버를 띄운다 (배경/준비 절차: docs/MAC_DEBUGGING.md)
#
# 기본 모드   : GPTK Wine + gdbserver.exe — D3D11(D3DMetal)이 살아있는 채로 전체 디버깅.
#              렌더 루프(매 프레임) 브레이크포인트 가능. 화면도 실제로 그려진다.
# --winedbg  : wine-devel + winedbg gdb 프록시 — D3D 디바이스 생성 불가(D3DMetal 없음).
#              gdbserver 도구가 준비 안 된 환경의 폴백.
#
# 사용:
#   scripts/mac-debug.sh [--winedbg] [exe경로]     # 기본 build-mac/RastertekSample.exe
# 접속(공통, 다른 터미널 또는 CLion "Remote Debug (winedbg)" 구성):
#   gdb <exe> → set architecture i386:x86-64 → target remote localhost:2159
set -euo pipefail
cd "$(dirname "$0")/.."

MODE=gdbserver
if [ "${1:-}" = "--winedbg" ]; then MODE=winedbg; shift; fi
EXE="${1:-build-mac/RastertekSample.exe}"
PORT="${PORT:-2159}"
[ -f "$EXE" ] || { echo "$EXE 없음 — 먼저 scripts/mac-build.sh 로 빌드하세요."; exit 1; }

# winedbg/gdbserver 는 유닉스식 경로를 못 받는다 — Windows식(Z:\...)으로 변환
WINPATH="Z:$(cd "$(dirname "$EXE")" && pwd | tr '/' '\\')\\$(basename "$EXE")"
CACHE="$HOME/Library/Caches/RastertekSample"

echo "== 접속 방법 (다른 터미널 또는 CLion Remote Debug):"
echo "   gdb $EXE"
echo "   (gdb) set architecture i386:x86-64"
echo "   (gdb) target remote localhost:$PORT"
echo

if [ "$MODE" = "gdbserver" ]; then
    GDBSRV="$CACHE/gdbsrv/gdbserver.exe"
    [ -f "$GDBSRV" ] || { echo "gdbserver 도구 없음: $CACHE/gdbsrv/ — docs/MAC_DEBUGGING.md의 '준비(1회)' 참고. 임시로는 --winedbg 모드 사용 가능."; exit 1; }
    command -v wine64 >/dev/null || { echo "wine64(GPTK)를 찾을 수 없습니다."; exit 1; }
    export WINEPREFIX="${WINEPREFIX:-$CACHE/wineprefix}"
    export WINEDEBUG="${WINEDEBUG:--all}"
    mkdir -p "$WINEPREFIX"
    echo "== [gdbserver + GPTK] D3D 살아있는 전체 디버깅 (port $PORT)"
    exec wine64 "$GDBSRV" "localhost:$PORT" "$WINPATH"
fi

# --winedbg 폴백: winedbg가 포함된 Wine 필요 (GPTK에는 없음)
WINE_BIN="${WINE_DEBUG_WINE:-}"
if [ -z "$WINE_BIN" ]; then
    for cand in "/Applications/Wine Devel.app/Contents/Resources/wine/bin/wine" \
                "/Applications/Wine Stable.app/Contents/Resources/wine/bin/wine"; do
        [ -x "$cand" ] && WINE_BIN="$cand" && break
    done
fi
[ -n "$WINE_BIN" ] || { echo "winedbg 포함 Wine을 못 찾음. WINE_DEBUG_WINE=<wine경로> 지정 또는 docs/MAC_DEBUGGING.md 참고."; exit 1; }
export WINEPREFIX="${WINEPREFIX:-$CACHE/wineprefix-debug}"
export WINEDEBUG="${WINEDEBUG:--all}"
mkdir -p "$WINEPREFIX"
echo "== [winedbg + wine-devel] 로직 전용 디버깅 — D3D 초기화는 실패함 (port $PORT)"
exec "$WINE_BIN" winedbg --gdb --no-start --port "$PORT" "$WINPATH"
