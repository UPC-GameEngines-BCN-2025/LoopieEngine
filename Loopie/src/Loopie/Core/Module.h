#pragma once

#include <string>

namespace Loopie {
	class Module {
	public:
		Module(const std::string& name = "DefaultModuleName", bool active = true);
		~Module() = default;

		virtual void OnLoad() {}
		virtual void OnUnload() {}
		virtual void OnUpdate() {}
		virtual void OnInterfaceRender() {}

		void Stop() { m_active = false; }
		void Start() { m_active = true; }

		const std::string& GetName() const { return m_name; }
		bool IsActive() const { return m_active; }
	private:
		std::string m_name;
		bool m_active = true;
	};
}