#pragma once
#include <cstddef>
namespace Loopie {
	class IIdentificable {
	public:
		IIdentificable() {}
		virtual ~IIdentificable() {}

		virtual size_t GetTypeID() const = 0;

		#define DEFINE_TYPE(Type) \
		static size_t GetTypeIDStatic() { \
			static const char typeUniqueVar = 0; \
			return reinterpret_cast<size_t>(&typeUniqueVar); \
		} \
		size_t GetTypeID() const override { \
			return GetTypeIDStatic(); \
		}
	};
}