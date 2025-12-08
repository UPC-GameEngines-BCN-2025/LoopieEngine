#pragma once 
#include "Editor/Interfaces/Interface.h"
#include "Loopie/Scene/Scene.h"

namespace Loopie {
	class HierarchyInterface : public Interface {
	public:
		HierarchyInterface();
		~HierarchyInterface() = default;
		void Init() override {}
		void Update(const InputEventManager& inputEvent) override;
		void Render() override;

		void SetScene(Scene* scene);

		void SelectEntity(std::shared_ptr<Entity> entity);

	private:
		void DrawEntitySlot(const std::shared_ptr<Entity>& entity);

		void DrawContextMenu(const std::shared_ptr<Entity>& entity);
		void HotKeysSelectedEntiy(const InputEventManager& inputEvent);

		void Drag(const std::shared_ptr<Entity>& entity);
		void Drop(const std::shared_ptr<Entity>& entity);

		std::shared_ptr<Entity> CreatePrimitiveModel(const std::string& modelPath, const std::string& name, const std::shared_ptr<Entity>& parent);

	public:
		static std::shared_ptr<Entity> s_SelectedEntity;
	private:
		Scene* m_scene = nullptr;
	};
}