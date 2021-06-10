#pragma once

// see https://hackernoon.com/bit-manipulation-in-c-and-c-1cs2bux

#include <cstdint>

constexpr std::uint_fast8_t mask0{ 0b0000'0001 }; // represents bit 0
constexpr std::uint_fast8_t mask1{ 0b0000'0010 }; // represents bit 1
constexpr std::uint_fast8_t mask2{ 0b0000'0100 }; // represents bit 2 
constexpr std::uint_fast8_t mask3{ 0b0000'1000 }; // represents bit 3
constexpr std::uint_fast8_t mask4{ 0b0001'0000 }; // represents bit 4
constexpr std::uint_fast8_t mask5{ 0b0010'0000 }; // represents bit 5
constexpr std::uint_fast8_t mask6{ 0b0100'0000 }; // represents bit 6
constexpr std::uint_fast8_t mask7{ 0b1000'0000 }; // represents bit 7

int set_bit(int num, int position)
{
	int mask = 1 << position;
	return num | mask;
}

bool get_bit(int num, int position)
{
	bool bit = num & (1 << position);
	return bit;
}

int clear_bit(int num, int position)
{
	int mask = 1 << position;
	return num & ~mask;
}