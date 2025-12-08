#pragma once
#include "Loopie/Events/IObserver.h"

#include <vector>
#include <algorithm>
#include <string>

namespace Loopie
{
    template<typename T>
    class Event {
    public:
        template<typename T>
        void AddObserver(IObserver<T>* obs) {
            observers.push_back(obs);
        }

        template<typename T>
        void RemoveObserver(IObserver<T>* obs) {
            observers.erase(std::remove(observers.begin(), observers.end(), obs), observers.end());
        }

        void Notify(const T& type) const {
            for (auto* obs : observers) {
                obs->OnNotify(type);
            }
        }

    private:
        std::vector<IObserver<T>*> observers;
    };
}