#pragma once

namespace compx {
namespace control {

class IControllable {
public:
	virtual auto start() -> void = 0;
	virtual auto release() -> void = 0;
	virtual auto end() -> void = 0;
};

}
}