#pragma once

#include <DirectXMath.h>

// 연산자를 XMINT2 와 같은 네임스페이스에 두어야 ADL 로 찾는다.
// 전역에 두면 다른 네임스페이스 안에서 operator 를 하나라도 선언했을 때 이름이 가려져 못 찾는다.
namespace DirectX
{
	constexpr XMINT2 operator+(XMINT2 a, XMINT2 b) noexcept { return XMINT2(a.x + b.x, a.y + b.y); }
	constexpr XMINT2 operator-(XMINT2 a, XMINT2 b) noexcept { return XMINT2(a.x - b.x, a.y - b.y); }
	constexpr XMINT2 operator-(XMINT2 a) noexcept { return XMINT2(-a.x, -a.y); }

	constexpr XMINT2 operator*(XMINT2 a, int32_t s) noexcept { return XMINT2(a.x * s, a.y * s); }
	constexpr XMINT2 operator*(int32_t s, XMINT2 a) noexcept { return a * s; }
	constexpr XMINT2 operator/(XMINT2 a, int32_t s) noexcept { return XMINT2(a.x / s, a.y / s); }

	constexpr XMINT2& operator+=(XMINT2& a, XMINT2 b) noexcept { a.x += b.x; a.y += b.y; return a; }
	constexpr XMINT2& operator-=(XMINT2& a, XMINT2 b) noexcept { a.x -= b.x; a.y -= b.y; return a; }
	constexpr XMINT2& operator*=(XMINT2& a, int32_t s) noexcept { a.x *= s; a.y *= s; return a; }
	constexpr XMINT2& operator/=(XMINT2& a, int32_t s) noexcept { a.x /= s; a.y /= s; return a; }

	// DirectXMath 는 이 조건일 때 XMINT2 에 멤버 == 를 기본 정의하므로, 같은 조건으로 비켜야 모호해지지 않는다.
	// MSVC 는 /Zc:__cplusplus 없이 __cplusplus 를 199711L 로 보고하므로 C++20 이어도 이쪽이 정의된다.
#if __cplusplus < 202002L
	constexpr bool operator==(XMINT2 a, XMINT2 b) noexcept { return a.x == b.x && a.y == b.y; }
	constexpr bool operator!=(XMINT2 a, XMINT2 b) noexcept { return !(a == b); }
#endif
}
