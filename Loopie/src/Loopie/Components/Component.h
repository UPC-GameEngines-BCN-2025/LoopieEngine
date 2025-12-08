#pragma once
#include <memory>

#include "Loopie/Core/IIdentificable.h"
#include "Loopie/Core/ISerializable.h"
#include "Loopie/Core/UUID.h"
#include "Loopie/Events/IObserver.h"
#include "Loopie/Files/Json.h"

//#include <nlohmann/json.hpp>

namespace Loopie {
	class Entity;
	class Transform;

	class Component : public IIdentificable, public ISerializable
	{
		friend class Entity;
	public:
		Component() = default;
		virtual ~Component();

		// Getters
		Transform* GetTransform() const;
		std::shared_ptr<Entity> GetOwner() const { return m_owner.lock(); }
		const UUID& GetUUID() const;
		bool GetIsActive() const;

		// Setters
		void SetIsActive(bool active);
		void SetUUID(const std::string uuid);

		// Serialize & Deserialize
		virtual JsonNode Serialize(JsonNode& parent) const =0;
		virtual void Deserialize(const JsonNode& data) =0;

		virtual void Init() = 0;

	private:
		std::weak_ptr<Entity> m_owner;
		UUID m_uuid;
		bool m_isActive = true;
	};
}