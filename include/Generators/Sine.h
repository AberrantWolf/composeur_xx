#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include "Util.h"
#include "../IGenerator.h"

namespace compx {
namespace gen {

class Sine :IGenerator {
public:
	Sine() : m_min(-1.0f), m_max(1.0f), m_freq(440.0f), m_distance(0.0f) {}

	virtual auto tick() -> TickResult override;

	virtual auto value() -> float override {
		return m_cache;
	}

	DECL_GENATTR(min);
	DECL_GENATTR(max);
	DECL_GENATTR(freq);

private:
	float m_distance;
	float m_cache;
};

}
}

#undef _USE_MATH_DEFINES
