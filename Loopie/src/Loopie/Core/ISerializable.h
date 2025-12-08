#pragma once
#include "Loopie/Files/Json.h"
namespace Loopie {
	class ISerializable {
	public:
		ISerializable() {}
		virtual ~ISerializable() {}

		virtual JsonNode Serialize(JsonNode& parent) const = 0;
		virtual void Deserialize(const JsonNode& data) = 0;
	};
}