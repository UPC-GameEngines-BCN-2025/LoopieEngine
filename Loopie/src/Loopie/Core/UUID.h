#pragma once

#include <string>
#include <cstddef>

namespace Loopie {
    class UUID {
    public:
        UUID();
        UUID(const std::string& id);

        const std::string& Get() const;
        static std::string Generate();

        bool operator==(const UUID& other) const;

    public:
        static const unsigned int UUID_SIZE = 36;

    private:
        std::string m_id;
    };
}

namespace std {
    template <>
    struct hash<Loopie::UUID> {
        std::size_t operator()(const Loopie::UUID& uuid) const noexcept {
            return std::hash<std::string>()(uuid.Get());
        }
    };
}