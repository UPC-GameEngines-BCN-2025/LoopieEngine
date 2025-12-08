#pragma once

namespace Loopie {
    enum class TransformNotification {
        OnDirty,
        OnChanged
    };

    enum class EngineNotification {
        OnProjectChange,
        OnAssetRegistryReload
    };
}
