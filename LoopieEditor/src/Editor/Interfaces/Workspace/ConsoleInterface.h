#pragma once 

#include "Editor/Interfaces/Interface.h"
#include "Loopie/Math/MathTypes.h"
#include <unordered_map>

namespace Loopie {
	class ConsoleInterface : public Interface {
	public:
		ConsoleInterface();
		~ConsoleInterface() = default;
		void Init() override;
		void Render() override;
	
	private:
		std::unordered_map<unsigned int, vec4> msgColor;
	};
}