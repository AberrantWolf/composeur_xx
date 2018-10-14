#pragma once

#include "IGenerator.h"

namespace compx {
namespace gen {

class IFrequencyGen : public IGenerator {
public:
	IFrequencyGen() : m_freq(440.0f) {}

	DECL_GENATTR(freq);
};

}
}