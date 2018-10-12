#include "Generators/Sine.h"

namespace compx {
namespace gen {

auto Sine::tick() -> TickResult {
	m_min.tick();
	m_max.tick();
	m_freq.tick();

	constexpr auto two_pi = static_cast<float>(2 * M_PI);
	const float step_size = two_pi * m_freq.get() / compx::util::SAMPLE_RATE;
	m_distance += step_size;

	if (m_distance > two_pi) {
		m_distance -= two_pi;
	}

	// sine wave between [0, 1]
	const auto val = (std::sin(m_distance) + 1) / 2.0f;
	// mapped to [min, max]
	const auto min = m_min.get();
	const auto max = m_max.get();
	m_cache = min + val * (max - min);

	return TickResult::OK;
}

}
}