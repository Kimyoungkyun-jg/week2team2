#pragma once

struct FLinearColor
{
	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;
	float a = 1.0f;

	constexpr FLinearColor() = default;
	constexpr FLinearColor(float InR, float InG, float InB, float InA = 1.0f)
		: r(InR), g(InG), b(InB), a(InA) {}

	static const FLinearColor Red;
	static const FLinearColor Green;
	static const FLinearColor Blue;
	static const FLinearColor White;
	static const FLinearColor Black;
	static const FLinearColor Yellow;
	static const FLinearColor Cyan;
	static const FLinearColor Magenta;
	static const FLinearColor Gray;
};

inline constexpr FLinearColor FLinearColor::Red     { 1.0f, 0.0f, 0.0f, 1.0f };
inline constexpr FLinearColor FLinearColor::Green   { 0.0f, 1.0f, 0.0f, 1.0f };
inline constexpr FLinearColor FLinearColor::Blue    { 0.0f, 0.0f, 1.0f, 1.0f };
inline constexpr FLinearColor FLinearColor::White   { 1.0f, 1.0f, 1.0f, 1.0f };
inline constexpr FLinearColor FLinearColor::Black   { 0.0f, 0.0f, 0.0f, 1.0f };
inline constexpr FLinearColor FLinearColor::Yellow  { 1.0f, 1.0f, 0.0f, 1.0f };
inline constexpr FLinearColor FLinearColor::Cyan    { 0.0f, 1.0f, 1.0f, 1.0f };
inline constexpr FLinearColor FLinearColor::Magenta { 1.0f, 0.0f, 1.0f, 1.0f };
inline constexpr FLinearColor FLinearColor::Gray    { 0.5f, 0.5f, 0.5f, 1.0f };
