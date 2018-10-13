#pragma once

#include "IGenerator.h"

namespace compx {
namespace gen {

class Constant:IGenerator {
public:
	Constant() : m_value(0.0f) {}
	Constant(float v) : m_value(v) {}

	virtual auto do_tick() -> TickResult override {
		return m_value.tick();
	}

	virtual auto calc_value() -> float override {
		return m_value.get();
	}

private:
	GenAttr m_value;
};

}
}