#pragma once 

#include "Editor/Interfaces/Interface.h"

namespace Loopie {
	class Transform;
	class Camera;
	class MeshRenderer;


	class InspectorInterface : public Interface {
	public:
		InspectorInterface();
		~InspectorInterface() = default;
		void Init() override {}
		void Render() override;

	private:
		void DrawEntityConfig(const std::shared_ptr<Entity>& entity);
		void DrawTransform(Transform* transform);
		void DrawCamera(Camera* camera);
		void DrawMeshRenderer(MeshRenderer* meshRenderer);

		void AddComponent(const std::shared_ptr<Entity>& entity);
	};
}