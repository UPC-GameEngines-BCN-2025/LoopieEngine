#pragma once
#include "Loopie/Core/Math.h"
#include "Loopie/Resources/Types/Material.h"
#include "Loopie/Resources/Types/Texture.h"
#include "Loopie/Render/VertexArray.h"
#include "Loopie/Render/UniformBuffer.h"
#include "Loopie/Components/Camera.h"

#include <filesystem>

namespace Loopie {
	class Transform;

	class Renderer {
	public:

		struct RenderItem {
			std::shared_ptr<VertexArray> VAO;
			unsigned int IndexCount;

			std::shared_ptr<Material> material;
			const Transform* transform;
		};
		
		// static void Init(void* (*getProcAddress)(const char*));
		using GLLoader = void* (*)(const char*);
		static void Init(GLLoader getProcAddress);
		static void Shutdown();

		static void Clear();
		static void SetClearColor(const vec4& color);
		static void SetViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

		static void RegisterCamera(Camera& camera);
		static void UnregisterCamera(Camera& camera);
		static const std::vector<Camera*>& GetRendererCameras() { return s_renderCameras; }

		static void BeginScene(const matrix4& viewMatrix, const matrix4& projectionMatrix, bool gizmo = true);
		static void EndScene();

		static void AddRenderItem(std::shared_ptr<VertexArray> vao, std::shared_ptr<Material> material, const Transform* transform);

		static void SetDefaultTexture(std::shared_ptr<Texture> texture) { s_DefaultTexture = texture; }

		static void EnableDepth();
		static void DisableDepth();

	private:
		static void SetRenderUniforms(std::shared_ptr<Material> material, const Transform* transform);
		static void FlushRenderQueue();

	public:
		static std::shared_ptr<Texture> s_DefaultTexture;
	private:
		static std::vector<RenderItem> s_RenderQueue;
		static std::vector<Camera*> s_renderCameras;
		static std::shared_ptr<UniformBuffer> s_matricesUniformBuffer;
	};
}