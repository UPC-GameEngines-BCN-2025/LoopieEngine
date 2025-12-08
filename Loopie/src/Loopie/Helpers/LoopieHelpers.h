#pragma once

#include <string>
#include <vector>

namespace Loopie {
    namespace Helper {
        inline std::string MakeUniqueName(const std::string& baseName, const std::vector<std::string>& existing, const std::string& sep = "_")
        {
            if (std::find(existing.begin(), existing.end(), baseName) == existing.end()) {
                return baseName;
            }

            int counter = 1;
            std::string candidate;

            while (true) {
                candidate = baseName + sep + std::to_string(counter);

                if (std::find(existing.begin(), existing.end(), candidate) == existing.end()) {
                    return candidate;
                }

                counter++;
            }
        }
    }
}