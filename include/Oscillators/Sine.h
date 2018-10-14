#pragma once

#include "Interfaces/IGenerator.h"
#include "Interfaces/IFrequencyGen.h"

namespace compx {
namespace osc {

class Sine : public gen::IFrequencyGen {
public:
	Sine() : IFrequencyGen(), m_min(-1.0f), m_max(1.0f), m_distance(0.0f) {}

	DECL_GENATTR(min);
	DECL_GENATTR(max);

protected:
	virtual auto do_tick(size_t id)->TickResult override;
	virtual auto calc_value() -> float override;

private:
	float m_distance;
};

}
}
