#pragma once

#include <memory>
#include <utility>
#include <variant>

namespace compx {

enum class TickResult {
	OK,
	COMPLETE,
	UNKNOWN,
	ERROR,
};

class IGenerator {
public:
	virtual ~IGenerator() {}

	virtual auto tick() -> TickResult final {
		m_cacheIsValid = false;
		return do_tick();
	};
	virtual auto value() -> float final {
		if (!m_cacheIsValid) {
			m_cache = calc_value();
			m_cacheIsValid = true;
		}
		return m_cache;
	};

protected:
	virtual auto do_tick() -> TickResult = 0;
	virtual auto calc_value() -> float = 0;

private:
	float m_cache;
	bool m_cacheIsValid;
};

using GenPtr = std::shared_ptr<IGenerator>;
using AttrValue = std::variant<float, GenPtr>;

struct GenAttr {
	auto somefunct() {
		AttrValue v{ GenPtr {} };
		v = 5.0f;
	}
private:
	AttrValue attribute;

public:
	GenAttr() : attribute(0.0f) {}
	GenAttr(float f) : attribute(f) {}
	GenAttr(GenPtr g) : attribute(g) {}

	auto get() -> float {
		switch (attribute.index()) {
		case 0:
			return std::get<0>(attribute);
		case 1:
			return std::get<1>(attribute)->value();;
			break;
		}
	}

	auto set(float f) {
		attribute = f;
	}
	
	auto set(GenPtr gen) {
		attribute = gen;
	}

	auto tick() -> TickResult {
		switch (attribute.index()) {
		case 0:
			break;
		case 1:
			return std::get<1>(attribute)->tick();
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

} // namespace compx
