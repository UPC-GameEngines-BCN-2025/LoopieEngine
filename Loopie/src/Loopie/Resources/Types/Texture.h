#pragma once
#include "Loopie/Resources/Resource.h"
#include "Loopie/Math/MathTypes.h"

#include "Loopie/Render/TextureBuffer.h"

#include <vector>
#include <memory>

namespace Loopie {

	class Texture : public Resource {
		friend class Material;
		friend class TextureImporter;
	public:
		DEFINE_TYPE(Texture)

		Texture(const UUID& id);
		~Texture() = default;

		static std::shared_ptr<Texture> GetDefault();

		bool Load() override;

		ivec2 GetSize() { return ivec2(m_width, m_height); }

		unsigned int GetRendererId() { return m_tb->GetRendererID(); }
	private:
		//std::vector<unsigned char> m_pixels;
		int m_width = 0;
		int m_height = 0;
		int m_channels = 0;

		std::shared_ptr<TextureBuffer> m_tb;

		static std::shared_ptr<Texture> s_Texture;
	};
}