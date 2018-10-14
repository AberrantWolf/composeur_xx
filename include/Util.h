#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include <string>

namespace compx {
namespace util {

struct Rate {
public:
	Rate(float r) : m_rate(r), m_inverse(1/r) {}

	auto steps_per_second() { return m_rate; }
	auto seconds_per_step() { return m_inverse; }

	auto set(float rate) {
		m_rate = rate;
		m_inverse = 1.0f / rate;
	}

private:
	float m_rate;
	float m_inverse;
};

extern Rate SAMPLE_RATE;

constexpr auto M_TAU_F = static_cast<float>(2 * M_PI);

auto get_pitch_from_note(std::string note_string) -> float;

}
}

#undef _USE_MATH_DEFINES
