#pragma once

#include <Windows.h>
#include <cstdio>
#include <type_traits>

//
// FAILED() 매크로를 대체하는 템플릿 기반 결과 검사.
//
// HRESULT 뿐 아니라 포인터(nullptr 검사), bool 도 같은 매크로로 처리한다.
// 지원하지 않는 타입을 넘기면 ResultTraits 가 정의되어 있지 않아 컴파일 단계에서 걸린다.
//
namespace Return
{
	// 결과 타입별 성공 판정 규칙. 새 타입은 아래에 특수화를 추가하면 매크로에서 그대로 쓸 수 있다.
	template<typename T, typename Enable = void>
	struct ResultTraits;

	// HRESULT, LONG 등 정수형 -> SUCCEEDED
	template<typename T>
	struct ResultTraits<T, typename std::enable_if<std::is_integral<T>::value && !std::is_same<T, bool>::value>::type>
	{
		static bool Succeeded(T value) { return SUCCEEDED(static_cast<HRESULT>(value)); }
	};

	// bool -> 그대로
	template<>
	struct ResultTraits<bool, void>
	{
		static bool Succeeded(bool value) { return value; }
	};

	// 모든 포인터 -> nullptr 검사
	template<typename T>
	struct ResultTraits<T*, void>
	{
		static bool Succeeded(T* value) { return value != nullptr; }
	};

	template<typename T>
	inline bool Succeeded(const T& value)
	{
		return ResultTraits<typename std::remove_cv<T>::type>::Succeeded(value);
	}

	inline void ReportFailure(const char* expression, const char* file, int line)
	{
		char message[512];
#ifdef _MSC_VER
		sprintf_s(message, "[CHECK FAILED] %s\n    %s(%d)\n", expression, file, line);
#else
		snprintf(message, sizeof(message), "[CHECK FAILED] %s\n    %s(%d)\n", expression, file, line);
#endif
		OutputDebugStringA(message);
	}
}

// if 문 안에서 직접 쓰는 형태
#define CHECK_SUCCEEDED(expr)	(::Return::Succeeded(expr))
#define CHECK_FAILED(expr)		(!::Return::Succeeded(expr))

// 실패 시 반환한다.
//   CHECK_RETURN(hr, false)  -> return false;   (반환값이 있는 함수)
//   CHECK_RETURN(hr)         -> return;         (void 함수)
#define CHECK_RETURN(expr, ...)									\
	do															\
	{															\
		if (!::Return::Succeeded(expr))							\
		{														\
			::Return::ReportFailure(#expr, __FILE__, __LINE__);	\
			return __VA_ARGS__;									\
		}														\
	} while (false)
