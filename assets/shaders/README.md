# assets/shaders

HLSL 셰이더 원본(`.vs` / `.ps`). `D3DCompileFromFile`로 **런타임 컴파일**한다 (fxc 사전 컴파일 금지 — Wine 호환).

로드 경로는 프로젝트 루트 기준 상대 경로로 쓴다:

```cpp
L"assets/shaders/color.vs"
```

Visual Studio의 작업 디렉터리(`$(ProjectDir)`)와 `scripts/mac-run.sh`(루트로 `cd` 후 실행) 모두 cwd가
프로젝트 루트이므로, 빌드 산출물 옆으로 복사하지 않아도 양쪽에서 그대로 동작한다.
