#pragma once

#include <utility>
#include <vector>

#include "Interfaces/IControllable.h"
#include "Interfaces/IGenerator.h"

namespace compx {
namespace fx {

class Envelope : public control::IControllable, public IGenerator {
public:
	struct EnvPhase {
		float offset;
		float value;
	};

	Envelope() : m_base(0.0f), m_source(0.0f) {}

	virtual auto do_tick(size_t id)->TickResult override;
	virtual auto calc_value() -> float override;

	// Append a phase to the end of the sequence.
	// This resets the envelope to the base value.
	auto appendPhase(EnvPhase data) {
		m_sequence.emplace_back(data);
		m_phase = m_sequence.size();
	}

	auto setSustainPhase(size_t id) {
		m_sustainPhase = id;
	}

	// Start playing the envelope.
	// The offset value of the first phase acts as a delay,
	// and the envelope will generate the base value until
	// the delay has elapsed.
	auto start() -> void override {
		m_phase = 0;
		m_phaseTime = 0.0f;
		m_holding = true;
	}

	// Jumps from wherever the envelope currently is to the last
	// phase in the sequence, and will blend between the phases.
	auto release() -> void override {
		m_holding = false;
		auto size = m_sequence.size();
		if (size > 1) {
			m_lastPhaseValue = value();
			m_phase = size - 1;
		}
	}

	// Jumps to the end of the sequence and plays the base value.
	auto end() -> void override{
		m_holding = false;
		m_phase = m_sequence.size();
	}

	DECL_GENATTR(base);
	DECL_GENATTR(source);

private:
	// offset, value
	std::vector<EnvPhase> m_sequence;
	size_t m_phase;
	float m_lastPhaseValue;
	size_t m_sustainPhase;
	float m_phaseTime;
	bool m_holding;
};

}
}
