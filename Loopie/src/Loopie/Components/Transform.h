#include "Loopie/Components/Component.h"
#include "Loopie/Scene/Entity.h"
#include "Loopie/Math/MathTypes.h"
#include "Loopie/Events/Event.h"
#include "Loopie/Events/EventTypes.h"

#include <memory>
namespace Loopie
{
    enum class ObjectSpace { Local, World };

    class Transform : public Component
    {
    public:
        DEFINE_TYPE(Transform)

        Transform(vec3 position = { 0,0,0 }, quaternion rotation = { 1, 0,0,0 }, vec3 scale = { 1,1,1 });
        ~Transform() = default;
        void Init()override;

        vec3 GetPosition();
        const vec3& GetLocalPosition() const;
        quaternion GetRotation();
        quaternion GetLocalRotation() const;
        vec3 GetEulerAngles();
        vec3 GetLocalEulerAngles() const;
        vec3 GetLocalScale() const;

        void SetPosition(const vec3& position);
        void SetLocalPosition(const vec3& position);
        void SetRotation(const quaternion& quat);
        void SetLocalRotation(const quaternion& quat);
        void SetEulerAngles(const vec3& euler_degrees);
        void SetLocalEulerAngles(const vec3& euler_degrees);
        void SetLocalScale(const vec3& scale);

		void SetWorldMatrix(const matrix4& worldMatrix);
        const matrix4& GetLocalToWorldMatrix() const;
        const matrix4& GetWorldToLocalMatrix() const;

        void SetWorldPosition(const vec3& position);
        void SetWorldRotation(const quaternion& quat);
        void SetWorldScale(const vec3& scale);

        void Translate(const vec3& translation, ObjectSpace space = ObjectSpace::Local);
        void Rotate(const vec3& euler_radians, ObjectSpace space = ObjectSpace::Local);
        void Rotate(const quaternion& delta, ObjectSpace space = ObjectSpace::Local);

        void LookAt(const vec3& worldTarget, const vec3& worldUp = vec3(0, 1, 0));

        vec3 Forward() const;
        vec3 Back() const;
        vec3 Up() const;
        vec3 Down() const;
        vec3 Right() const;
        vec3 Left() const;

        void MarkLocalDirty();
        void MarkWorldDirty();
        bool IsDirty() const;

        void ForceRefreshMatrices();

        // Serialize & Deserialize
        JsonNode Serialize(JsonNode& parent) const override;
        void Deserialize(const JsonNode& data) override;
        

    private:
        vec3 GetWorldPosition() const;
        quaternion GetWorldRotation() const;
        vec3 GetWorldScale() const;

        void RefreshMatrices() const;

    public:
        Event<TransformNotification> m_transformNotifier;

    private:
        vec3 m_localPosition = vec3(0);
        quaternion m_localRotation = quaternion(1, 0, 0, 0);
        vec3 m_localScale = vec3(1);

        mutable glm::vec3 m_cachedEuler = glm::vec3(0.0f);
        mutable bool m_cachedEulerDirty = true;

        mutable matrix4 m_localToWorld = matrix4(1);
        mutable matrix4 m_worldToLocal = matrix4(1);

        mutable bool m_localDirty = true;
        mutable bool m_worldDirty = true;
    };
}