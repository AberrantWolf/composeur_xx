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

	virtual auto tick(size_t id) -> TickResult final {
		if (id == m_lastTick) {
			return TickResult::OK;
		}
		m_cacheIsValid = false;
		m_lastTick = id;
		return do_tick(id);
	};
	virtual auto value() -> float final {
		if (!m_cacheIsValid) {
			m_cache = calc_value();
			m_cacheIsValid = true;
		}
		return m_cache;
	};

protected:
	virtual auto do_tick(size_t id) -> TickResult = 0;
	virtual auto calc_value() -> float = 0;

private:
	size_t m_lastTick;
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
		return 0.0f;
	}

	auto set(float f) {
		attribute = f;
	}
	
	auto set(GenPtr gen) {
		attribute = gen;
	}

	auto tick(size_t id) -> TickResult {
		switch (attribute.index()) {
		case 0:
			break;
		case 1:
			return std::get<1>(attribute)->tick(id);
			break;
		default:
			return TickResult::UNKNOWN;
		}

		return TickResult::OK;
	}
};

#define DECL_GENATTR(ATTR_NAME) \
protected: GenAttr m_##ATTR_NAME;\
public:\
	auto set_##ATTR_NAME(float f) {m_##ATTR_NAME.set(f);}\
	auto set_##ATTR_NAME(GenPtr g) {m_##ATTR_NAME.set(g);}

} // namespace compx
