#include <iostream>

#include "Effects/Envelope.h"
#include "Util.h"

namespace compx {
namespace fx {

uint32_t count = 0;

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

	//EnvPhase phase_data;
	while (m_phase < m_sequence.size()) {
		auto phase_data = m_sequence.at(m_phase);
		if (m_phaseTime < phase_data.offset || (m_holding && (m_phase == m_sustainPhase))) {
			break;
		}
		m_lastPhaseValue = phase_data.value;
		m_phaseTime -= phase_data.offset;
		m_phase++;
		std::cout << "changing phase " << count << std::endl;
		count = 0;
	}

	return TickResult::OK;
}

// 0>>              1>> 2>> 3>> (hold) 4>> !
//                  |   |_  |          |   |
// (initial delay)  | _/| \_|__________|   | (== end)
// _________________|/  |   |          |\__|__________
auto Envelope::calc_value() -> float {
	++count;
	if (m_phase == 0 || m_phase >= m_sequence.size()) {
		return m_base.get() * m_source.get();
	}

	const auto phase_data = m_sequence[m_phase];
	const auto ratio = std::fmax(0.0f, std::fmin(m_phaseTime / phase_data.offset, 1.0f));

	const auto min = m_lastPhaseValue;
	const auto max = phase_data.value;

	return (min + ratio * (max - min)) * m_source.get();
}

}
}
