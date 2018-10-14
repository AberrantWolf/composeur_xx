#pragma once

#include <memory>
#include <vector>

#include "Interfaces/IGenerator.h"

namespace compx {
namespace fx {

class Mixer : public IGenerator {
public:
	virtual auto do_tick(size_t id)->TickResult override {
		for each (auto source in m_sources)
		{
			source.gen->tick(id);
		}
		return TickResult::OK;
	}
	virtual auto calc_value() -> float override {
		float result = 0.0f;
		for each (auto source in m_sources)
		{
			result += source.gen->value() * source.amplitude;
		}
		return result;
	}

	auto add_generator(std::shared_ptr<IGenerator> gen, float amplitude) {
		m_sources.push_back({ gen, amplitude });
	}

	auto remove_generator(std::shared_ptr<IGenerator> gen) {
		auto found = std::find(m_sources.begin(), m_sources.end(), gen);
		if (found != m_sources.end()) {
			m_sources.erase(found);
		}
	}

private:
	struct MixSource {
		std::shared_ptr<IGenerator> gen;
		float amplitude;
		
		bool operator==(const std::shared_ptr<IGenerator> rhs) const {
			return gen == rhs;
		}
	};
	std::vector<MixSource> m_sources;
};

}
}
