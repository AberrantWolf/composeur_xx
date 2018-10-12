#pragma once

#include <memory>

namespace compx {
namespace gen {

enum class TickResult {
	OK,
	COMPLETE,
	UNKNOWN,
	ERROR,
};

class IGenerator {
public:
	virtual ~IGenerator() {}

	virtual auto tick() -> TickResult = 0;
	virtual auto value() -> float = 0;
};

using GenPtr = std::shared_ptr<IGenerator>;

struct GenAttr {
private:
	union AttrValue {
		float value;
		GenPtr generator;

		AttrValue(float f) : value(f) {}
		AttrValue(GenPtr g) : generator(g) {}
		~AttrValue() {}
	} attribute;

	enum {
		VALUE,
		GENERATOR,
	} type;

public:
	GenAttr(float f) : attribute(f), type(VALUE) {}
	GenAttr(GenPtr g) : attribute(g), type(GENERATOR) {}

	auto get() -> float {
		switch (type) {
		case VALUE:
			return attribute.value;
			break;
		case GENERATOR:
			return attribute.generator->value();
		}
	}

	auto set(float f) {
		attribute.value = f;
	}
	
	auto set(GenPtr gen) {
		attribute.generator = gen;
	}

	auto tick() -> TickResult {
		switch (type) {
		case VALUE:
			break;
		case GENERATOR:
			return attribute.generator->tick();
			break;
		default:
			return TickResult::UNKNOWN;
		}

		return TickResult::OK;
	}
};

#define DECL_GENATTR(ATTR_NAME) \
private: GenAttr m_##ATTR_NAME;\
public:\
	auto set_##ATTR_NAME(float f) {m_##ATTR_NAME.set(f);}\
	auto set_##ATTR_NAME(GenPtr g) {m_##ATTR_NAME.set(g);}

} // namespace gen
} // namespace compx
