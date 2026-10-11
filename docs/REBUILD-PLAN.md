# 재구축 계획 — "Rastertek은 스펙, 코드는 내 것"

작성 2026-08-23 · 상태: **계획 단계, 미착수**

## 배경

- 학습 노트(`C:\GitRepositories\DirectX\Rastertek\Docs`, tut01~61)는 Rastertek 원문 코드 verbatim 기준.
- 원문 코드는 2010년 스타일(raw 포인터·`0`·빈 생성자/소멸자·`Initialize→InitializeShader` 2단·셰이더 클래스 20개 복붙)이라 학습에 걸림돌.
- 이 리포는 tut05 진행 중(약 2,000줄) — 방향을 바꾸기에 가장 싼 시점.

## 결정 사항

| # | 결정 | 상태 |
|---|---|---|
| a | COM은 `Microsoft::WRL::ComPtr`, 내 객체는 `std::unique_ptr` (`Shutdown` 계열·`SafeRelease`·`new/delete` 은퇴) | 제안, 미확정 |
| b | `Class` 접미사 제거 (`ModelClass`→`Model`) — 원문과의 대응은 표 한 줄로 | 제안, 미확정 |
| c | 문서의 코드 영역을 **이 리포의 git 태그 기준으로 생성** (원문 verbatim 폐기) | 제안, 미확정 |
| d | 문서의 설명·다이어그램·퀴즈 섹션은 유지, 코드 섹션만 교체 | 제안 |
| e | tut02~04 상태는 커밋으로 재구성해 태그를 붙인다 (따라 치기가 2편부터 일관되도록) | 제안 |

## 0단계 — 규약 확정

산출물: `docs/CODING_STYLE.md` (+ `.editorconfig`, 가능하면 `.clang-format`)

| 항목 | 규약 |
|---|---|
| 소유 | COM 객체 `ComPtr<T>`, 내 클래스 객체 `std::unique_ptr<T>`. `shared_ptr` 안 씀. raw `T*`/`T&`는 빌리는 매개변수에만. `new`/`delete` 직접 호출 금지 |
| 초기화 | 헤더에서 멤버 기본 초기화. 생성자·소멸자·복사생성자 안 씀, 복사는 `= delete` |
| Initialize | 직접 일함. 내부 위임(`InitializeShader`) 금지. 실패는 `bool` 반환 |
| 에러 | `CHECK_RETURN(expr, false)` 일관. `if (FAILED(hr)) return false;` 금지 |
| 문자열 | 경로 `std::filesystem::path` 또는 `const wchar_t*`. 보관은 `std::wstring`. 비-const `char*` 금지 |
| 헤더 | 매개변수 이름 항상 표기. 한 줄 게터는 인라인 `const` |
| 군더더기 | void 끝 `return;`, 블록 뒤 빈 줄 2개, `using namespace std` in 헤더 금지 |
| 줄 길이 | **120자 상한** (`.clang-format` `ColumnLimit`). 문서 코드 블록이 오토래핑이라 긴 줄은 읽기 깨짐 |
| 네이밍 | 타입·함수·파일 Pascal / **멤버 `_camel`** (`_` 뒤 소문자만, `__` 금지 — 그 외엔 클래스 스코프에서 합법) / 로컬·매개변수 camel / 상수·enum 값 Pascal(매크로만 UPPER) / bool 접두사 없음. 폴더 `Core/Graphics/Shaders/Text/Utility` 유지. (2026-08-23 확정: `m_`는 번거롭고, Unreal식 무접두사 Pascal은 멤버/로컬 구분이 약해서 제외) |
| 추상화 규칙 | **현재 튜토리얼이 요구하지 않는 추상화는 만들지 않는다. 중복은 3번째에서 뽑는다** (엔진화 함정 방지) |
| 크로스컴파일 | 기존 CLAUDE.md 규칙 그대로 (`#pragma comment` 금지, BOM 규칙, 런타임 HLSL 컴파일) |

## 1단계 — 기반 재작성 (tut02~05)

1. `Core/`: `System`, `Input`, `Application` — 규약 적용
2. `Graphics/`: `D3D`, `Model`, `Camera`, `Texture` — ComPtr, 인라인 게터
3. `Shaders/`: `ShaderBase` 추출
   - 베이스: `.vs/.ps` 컴파일 + `shader-error.txt` 출력, `MatrixBuffer` 생성/갱신, `Render(context, indexCount)` 바인딩
   - 파생이 제공: 입력 레이아웃 배열, 추가 cbuffer, 샘플러, 텍스처 바인딩
   - `ColorShader`, `TextureShader`를 그 위에
4. vcxproj(+filters)·CMakeLists 양쪽 갱신
5. Windows + mac(Wine) 빌드·실행 확인
6. 태그: tut02~05 상태를 커밋으로 재구성 후 `tut02`…`tut05` 태그 (결정 e)

완료 정의: 두 플랫폼에서 삼각형(04)·텍스처(05) 그대로 출력, `git diff tut04..tut05`가 "튜토리얼 5 변경분"을 정확히 보여줌.

## 2단계 — 문서 생성 파이프라인

산출물: `Rastertek/Docs/tools/gen-code-sections.py` (+ 검증 스크립트)

입력: 태그 두 개(`tutN-1`, `tutN`). 출력: 해당 HTML의 코드 섹션.
- **신규 파일** → 전체 코드 블록(`pre.newfile` 뱃지, 한글 주석)
- **수정 파일** → 헝크(함수 라벨 + 문맥 2줄 `pre.ctx`, 삭제는 `pre.del`). 지금의 수작업 diffview 199개는 폐기
- **따라 치기 순서 고정**: 헤더 → cpp(함수 순서) → HLSL → 호출부(`Application`)
- HTML에는 `<!-- code:begin modelclass -->` 식 마커를 두고 그 사이만 교체 → 설명 섹션은 보존
- 검증: 태그 사이 변경 파일이 전부 문서에 등장하는지, 스니펫이 실제 소스와 바이트 단위로 일치하는지
- **표시 규칙: 코드 블록은 가로 스크롤 대신 오토래핑** (`pre { white-space: pre-wrap; overflow-wrap: anywhere; }`). 래핑된 줄이 구분되도록 이어지는 줄 들여쓰기(`padding-left` + `text-indent` 음수) 적용. 코드 쪽 120자 상한과 짝

tut05로 프로토타입 → tut04·03·02 순으로 소급 적용.

주의: 설명 글이 원문 코드를 언급하는 곳(예: "Shutdown 함수에서…")은 코드가 바뀌면 틀린 문장이 된다. 생성 시 **변경 파일과 같은 섹션의 산문을 목록으로 뽑아** 손으로 검토하는 단계를 둔다.

## 3단계 — 튜토리얼 사이클 (tut06~)

튜토리얼 하나의 완료 정의:
1. 코드 작성 (규약 준수, 추상화 규칙 준수)
2. Windows + mac 빌드·실행, 결과 화면 확인
3. `tutNN` 태그
4. 파이프라인으로 문서 코드 섹션 생성 + 검증 통과
5. 산문 검토 목록 처리
6. 커밋 메시지 `TUTORIAL NN · 제목` (현재 관행 유지)

체크포인트:
- **tut10 끝**: 규약이 실제로 편한지, 파이프라인이 돌아가는지 재평가. 여기까지가 MVP
- **tut21 끝**: 22·23·25가 원문에서 비선형(구버전 클래스 요구). 내 코드는 선형이므로 문서 내용이 원문과 갈라지는 첫 지점 — 원문과 다른 부분을 문서에 명시하는 방식 결정
- **tut49·50**: 테셀레이션·디퍼드에서 `ShaderBase`가 버티는지. 안 버티면 그때 확장(미리 하지 않음)

## 리스크

| 리스크 | 대응 |
|---|---|
| 엔진 만들다 D3D11 공부를 못 함 | 추상화 규칙(3번째 중복). 체크포인트에서 "튜토리얼 진도"를 지표로 |
| 61편 × (코드+문서)는 길다 | MVP = tut10. 거기서 계속할지 결정 |
| 산문과 코드 불일치 | 2단계의 산문 검토 목록. 검증 스크립트로 스니펫 일치 강제 |
| ComPtr mingw 호환 | `wrl/client.h`는 mingw-w64에 있음. 1단계 mac 빌드에서 바로 검증 |
| 문서 재생성 중 기존 내용 훼손 | 마커 사이만 교체. 생성 전후 diff를 항상 확인 |
