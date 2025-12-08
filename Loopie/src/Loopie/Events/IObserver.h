#pragma once
#include <string>

namespace Loopie 
{
    template<typename T>
    class IObserver {
    public:
        virtual ~IObserver() = default;

        virtual void OnNotify(const T& id) = 0;
    };
}