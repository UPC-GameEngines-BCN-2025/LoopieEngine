#include "Material.h"

#include "Loopie/Resources/ResourceManager.h"
#include "Loopie/Importers/MaterialImporter.h"

#include "Loopie/Core/Log.h"
#include "Loopie/Render/Renderer.h"

namespace Loopie
{

	std::shared_ptr<Material> Material::s_Material = nullptr;

	Material::Material(const UUID& id) : Resource(id, ResourceType::MATERIAL)
	{
		ResetMaterial();
		Load();
	}

	Material::~Material()
	{

	}

	std::shared_ptr<Material> Material::GetDefault()
	{
		if (s_Material)
			return s_Material;
		Metadata& metadata = AssetRegistry::GetOrCreateMetadata("assets/materials/defaultMaterial.mat");
		if (!metadata.HasCache) {
			MaterialImporter::ImportMaterial("assets/textures/simpleWhiteTexture.png", metadata);
		}
		s_Material = ResourceManager::GetMaterial(metadata);
		s_Material->Load();
		s_Material->m_editable = false;
		return s_Material;
	}

	void Material::Bind()
	{
		if (!m_shader.GetIsValidShader())
		{
			Log::Error("Cannot apply material with invalid shader.");
			return;
		}

		m_shader.Bind();

		if (m_texture)
		{
			m_texture->m_tb->Bind();
		}
		else
		{
			Texture::GetDefault()->m_tb->Bind();
		}

		for (const auto& [name, uniformValue] : m_uniformValues)
		{
			ApplyUniform(name, uniformValue);
		}
	}

	void Material::Unbind() const
	{
		m_shader.Unbind();
	}

	bool Material::Load()
	{
		Metadata* metadata = AssetRegistry::GetMetadata(GetUUID());
		if (metadata->HasCache) {
			MaterialImporter::LoadMaterial(metadata->CachesPath[0], *this);
			return true;
		}
		return false;
	}

	void Material::Save()
	{
		//// Save
		Metadata* metadata = AssetRegistry::GetMetadata(GetUUID());
		if (metadata) {
			MaterialImporter::SaveMaterial(AssetRegistry::GetSourcePath(GetUUID()), *this, *metadata);
			AssetRegistry::RefreshAssetRegistry();
			Load();
		}
	}

	void Material::ResetMaterial()
	{
		if (!m_shader.GetIsValidShader())
		{
			Log::Error("Cannot reset material with invalid shader.");
			return;
		}

		m_uniformValues.clear();
		const auto& uniforms = m_shader.GetUniforms();
		for (const auto& uniform : uniforms)
		{
			m_uniformValues[uniform.id].type = uniform.type;
			m_uniformValues[uniform.id].value = uniform.defaultValue;
		}

		Log::Info("Material reset to default values");
	}

	void Material::ApplyUniform(const std::string& name, const UniformValue& uniformValue)
	{
		switch (uniformValue.type)
		{
		case UniformType_float:m_shader.SetUniformFloat(name, std::get<float>(uniformValue.value));		break;
		case UniformType_int:  m_shader.SetUniformInt(name, std::get<int>(uniformValue.value));			break;
		case UniformType_uint: m_shader.SetUniformInt(name, static_cast<int>(std::get<unsigned int>(uniformValue.value)));	break;
		case UniformType_bool: m_shader.SetUniformInt(name, std::get<bool>(uniformValue.value) ? 1 : 0);break;
		case UniformType_vec2: m_shader.SetUniformVec2(name, std::get<vec2>(uniformValue.value));		break;
		case UniformType_vec3: m_shader.SetUniformVec3(name, std::get<vec3>(uniformValue.value));		break;
		case UniformType_vec4: m_shader.SetUniformVec4(name, std::get<vec4>(uniformValue.value));		break;
		case UniformType_mat2: m_shader.SetUniformMat2(name, std::get<matrix2>(uniformValue.value));	break;
		case UniformType_mat3: m_shader.SetUniformMat3(name, std::get<matrix3>(uniformValue.value));	break;
		case UniformType_mat4: m_shader.SetUniformMat4(name, std::get<matrix4>(uniformValue.value));	break;
		case UniformType_Sampler2D: 	break;
		case UniformType_Sampler3D: 	break;
		default: Log::Warn("Unknown uniform type for '{0}'", name);	break;
		}
	}

	UniformValue* Material::GetShaderVariable(const std::string& name)
	{
		auto it = m_uniformValues.find(name);
		if (it != m_uniformValues.end())
		{
			return &it->second;
		}

		return nullptr;
	}

	void Material::SetShader(const Shader& shader)
	{
		if (!shader.GetIsValidShader())
		{
			Log::Error("Cannot set invalid shader to material.");
			return;
		}

		m_shader = shader;
		ResetMaterial();
	}

	bool Material::SetShaderVariable(const std::string& name, const UniformValue& value)
	{
		if (!m_editable)
			return false;
		auto it = m_uniformValues.find(name);
		if (it == m_uniformValues.end())
		{
			Log::Warn("Uniform '{0}' not found in material.", name);
			return false;
		}

		if (it->second.type != value.type)
		{
			Log::Error("Type mismatch for uniform '{0}'. Expected type {1}, got type {2}",
				name, static_cast<int>(it->second.type), static_cast<int>(value.type));
			return false;
		}

		it->second = value;
		return true;
	}

	void Material::SetTexture(std::shared_ptr<Texture> texture)
	{
		if (!m_editable)
			return;
		m_texture = texture;
	}
}