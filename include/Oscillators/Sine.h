#pragma once


#include "Util.h"
#include "../IGenerator.h"

namespace compx {
namespace osc {

class Sine : public IGenerator {
public:
	Sine() : m_min(-1.0f), m_max(1.0f), m_freq(440.0f), m_distance(0.0f) {}

	DECL_GENATTR(min);
	DECL_GENATTR(max);
	DECL_GENATTR(freq);

protected:
	virtual auto do_tick()->TickResult override;
	virtual auto calc_value() -> float override;

private:
	float m_distance;
};

}
}
