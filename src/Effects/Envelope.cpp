#include "Effects/Envelope.h"
#include "Util.h"

namespace compx {
namespace fx {

auto Envelope::do_tick() -> TickResult {
	m_base.tick();
	m_source.tick();

	if (m_phase >= m_sequence.size()) {
		return TickResult::OK;
	}

	// increment the phaseTime
	m_phaseTime += compx::util::SAMPLE_RATE.seconds_per_step();

	if (m_holding && m_phase == m_sustainPhase) {
		return TickResult::OK;
	}

	for (auto phase_data = m_sequence.at(m_phase)
		; m_phaseTime >= phase_data.offset && m_phase < m_sequence.size()
		; m_phase++) {
		m_lastPhaseValue = phase_data.value;
		m_phaseTime -= phase_data.offset;
	}

	return TickResult::OK;
}

// 0>>              1>> 2>> 3>> (hold) 4>> !
//                  |   |_  |          |   |
// (initial delay)  | _/| \_|__________|   | (== end)
// _________________|/  |   |          |\__|__________
auto Envelope::calc_value() -> float {
	if (m_phase == 0 || m_phase >= m_sequence.size()) {
		return m_base.get() * m_source.get();
	}

	const auto phase_data = m_sequence[m_phase];
	const auto ratio = std::fmin(m_phaseTime / phase_data.offset, 1.0f);

	const auto min = m_lastPhaseValue;
	const auto max = phase_data.value;

	return (min + ratio * (max - min)) * m_source.get();
}

}
}
