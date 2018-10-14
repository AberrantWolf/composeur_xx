#include "Oscillators/Sine.h"
#include "Util.h"

namespace compx {
namespace osc {

auto Sine::do_tick(size_t id) -> TickResult {
	m_min.tick(id);
	m_max.tick(id);
	m_freq.tick(id);

	return TickResult::OK;
}

auto Sine::calc_value() -> float {
	const float step_size = util::M_TAU_F * m_freq.get() * compx::util::SAMPLE_RATE.seconds_per_step();
	m_distance += step_size;

	if (m_distance > util::M_TAU_F) {
		m_distance -= util::M_TAU_F;
	}

	// sine wave between [0, 1]
	const auto val = (std::sin(m_distance) + 1) / 2.0f;
	// mapped to [min, max]
	const auto min = m_min.get();
	const auto max = m_max.get();
	return min + val * (max - min);
}

}
}