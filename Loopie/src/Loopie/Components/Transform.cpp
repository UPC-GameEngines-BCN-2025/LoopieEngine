#include "Transform.h"
#include "Loopie/Components/Component.h"
#include "Loopie/Scene/Entity.h"
#include "Loopie/Math/MathUtils.h"
#include <memory>
namespace Loopie
{
    Transform::Transform(vec3 position, quaternion rotation, vec3 scale)
    {
        m_localPosition = position;
        m_localRotation = rotation;
        m_localScale = scale;
    };

    void Transform::Init() 
    {
        ForceRefreshMatrices();
    }

    const vec3& Transform::GetLocalPosition() const
    {
        return m_localPosition;
    }

    void Transform::SetLocalPosition(const vec3& position)
    {
        m_localPosition = position;
        MarkLocalDirty();
    }

    quaternion Transform::GetLocalRotation() const
    {
        return m_localRotation;
    }

    void Transform::SetLocalRotation(const quaternion& quat)
    {
        m_localRotation = normalize(quat);
        m_cachedEulerDirty = true;
        MarkLocalDirty();
    }

    vec3 Transform::GetLocalScale() const
    {
        return m_localScale;
    }

    void Transform::SetLocalScale(const vec3& scale)
    {
        m_localScale = scale;
        MarkLocalDirty();
    }

    vec3 Transform::GetPosition()
    {
        return GetWorldPosition();
    }

    void Transform::SetPosition(const vec3& position)
    {
        SetWorldPosition(position);
    }

    quaternion Transform::GetRotation()
    {
        return GetWorldRotation();
    }

    void Transform::SetRotation(const quaternion& quat)
    {
        SetWorldRotation(quat);
    }

    vec3 Transform::GetLocalEulerAngles() const
    {
        if (m_cachedEulerDirty)
        {
            m_cachedEuler = degrees(eulerAngles(m_localRotation));
            m_cachedEulerDirty = false;
        }
        return m_cachedEuler;
    }

    void Transform::SetLocalEulerAngles(const vec3& euler_degrees)
    {
        m_localRotation = quaternion(radians(euler_degrees));
        m_cachedEuler = euler_degrees;
        m_cachedEulerDirty = false;
        MarkLocalDirty();
    }

    vec3 Transform::GetEulerAngles()
    {
        if (auto parent = GetOwner()->GetParent().lock())
        {
            quaternion worldRot = GetRotation();
            return degrees(eulerAngles(worldRot));
        }
        return degrees(eulerAngles(m_localRotation));
    }

    void Transform::SetEulerAngles(const vec3& euler_degrees)
    {
        if (auto parent = GetOwner()->GetParent().lock())
        {
            quaternion parentRot = parent->GetTransform()->GetRotation();
            quaternion worldRot = quaternion(radians(euler_degrees));
            m_localRotation = inverse(parentRot) * worldRot;
        }
        else
        {
            m_localRotation = quaternion(radians(euler_degrees));
        }
        MarkLocalDirty();
    }

    void Transform::SetWorldMatrix(const matrix4& worldMatrix)
    {
        vec3 position, scale;
        quaternion rotation;

        Math::DecomposeMatrix(worldMatrix, position, rotation, scale);

        if (auto parent = GetOwner()->GetParent().lock())
        {
            Transform* parentTransform = parent->GetTransform();

            SetLocalPosition(vec3(parentTransform->GetWorldToLocalMatrix() * vec4(position, 1.0f)));
            SetLocalRotation(inverse(parentTransform->GetWorldRotation()) * rotation);
            SetLocalScale(scale / parentTransform->GetWorldScale());
        }
        else
        {
            SetLocalPosition(position);
            SetLocalRotation(rotation);
            SetLocalScale(scale);
        }
    }

    const matrix4& Transform::GetLocalToWorldMatrix() const
    {
        RefreshMatrices();
        return m_localToWorld;
    }

    const matrix4& Transform::GetWorldToLocalMatrix() const
    {
        RefreshMatrices();
        return m_worldToLocal;
    }

    vec3 Transform::GetWorldPosition() const
    {
        RefreshMatrices();
        return vec3(m_localToWorld[3]);
    }

    quaternion Transform::GetWorldRotation() const
    {
        RefreshMatrices();

        matrix3 rotMat = matrix3(m_localToWorld);
        vec3 scale(length(rotMat[0]), length(rotMat[1]), length(rotMat[2]));
        rotMat[0] /= scale.x;
        rotMat[1] /= scale.y;
        rotMat[2] /= scale.z;

        return Math::ToQuaternion(rotMat);
    }

    vec3 Transform::GetWorldScale() const
    {
        RefreshMatrices();
        vec3 wx = vec3(m_localToWorld[0]);
        vec3 wy = vec3(m_localToWorld[1]);
        vec3 wz = vec3(m_localToWorld[2]);
        return vec3(length(wx), length(wy), length(wz));
    }

    void Transform::SetWorldPosition(const vec3& position)
    {
        if (auto parent = GetOwner()->GetParent().lock())
        {
            Transform* transform = parent->GetTransform();
            vec3 local = vec3(transform->GetWorldToLocalMatrix() * vec4(position, 1.0f));
            SetLocalPosition(local);
        }
        else
        {
            SetLocalPosition(position);
        }
    }

    void Transform::SetWorldRotation(const quaternion& quat)
    {
        if (auto parent = GetOwner()->GetParent().lock())
        {
            Transform* transform = parent->GetTransform();
            quaternion parentWorld = transform->GetWorldRotation();
            quaternion local = inverse(parentWorld) * quat;
            SetLocalRotation(local);
        }
        else
        {
            SetLocalRotation(quat);
        }
    }

    void Transform::SetWorldScale(const vec3& scale)
    {
        if (auto parent = GetOwner()->GetParent().lock())
        {
            Transform* transform = parent->GetTransform();
            vec3 parentScale = transform->GetWorldScale();
            vec3 local = scale / parentScale;
            SetLocalScale(local);
        }
        else
        {
            SetLocalScale(scale);
        }
    }

    void Transform::Translate(const vec3& translation, ObjectSpace space)
    {
        if (space == ObjectSpace::Local)
        {
            vec3 t = m_localRotation * translation;
            SetLocalPosition(m_localPosition + t);
        }
        else
        {
            if (auto parent = GetOwner()->GetParent().lock())
            {
                Transform* transform = parent->GetTransform();
                vec3 localT = vec3(transform->GetWorldToLocalMatrix() * vec4(translation, 0.0f));
                SetLocalPosition(m_localPosition + localT);
            }
            else
            {
                SetLocalPosition(m_localPosition + translation);
            }
        }
    }

    void Transform::Rotate(const vec3& eulerRadians, ObjectSpace space)
    {
        quaternion quat = quaternion(eulerRadians);
        Rotate(quat, space);
    }

    void Transform::Rotate(const quaternion& delta, ObjectSpace space)
    {
        if (space == ObjectSpace::Local) {
            SetLocalRotation(m_localRotation * normalize(delta));
        }
        else {
            quaternion worldRot = GetWorldRotation();
            quaternion newWorld = normalize(delta * worldRot);
            SetWorldRotation(newWorld);
        }
    }

    void Transform::LookAt(const vec3& worldTarget, const vec3& worldUp)
    {
        vec3 pos = GetWorldPosition();
        matrix4 look = lookAt(pos, worldTarget, worldUp);
        matrix4 rotMat = inverse(look);
        quaternion quat = Math::ToQuaternion(matrix3(rotMat));
        SetWorldRotation(quat);
    }

    vec3 Transform::Forward() const
    {
        return GetWorldRotation() * vec3(0.0f, 0.0f, -1.0f);
    }

    vec3 Transform::Back() const
    {
        return -Forward();
    }

    vec3 Transform::Up() const
    {
        return GetWorldRotation() * vec3(0.0f, 1.0f, 0.0f);
    }

    vec3 Transform::Down() const
    {
        return -Up();
    }

    vec3 Transform::Right() const
    {
        return GetWorldRotation() * vec3(1.0f, 0.0f, 0.0f);
    }

    vec3 Transform::Left() const
    {
        return -Right();
    }

    void Transform::MarkLocalDirty()
    {
        m_localDirty = true;
        MarkWorldDirty();
    }

    void Transform::MarkWorldDirty()
    {
        if (m_worldDirty) 
            return;
        m_worldDirty = true;
        m_transformNotifier.Notify(TransformNotification::OnDirty);

        for (auto& child : GetOwner()->GetChildren()) {
            if (child) child->GetTransform()->MarkWorldDirty();
        }

    }

    bool Transform::IsDirty() const{
        return m_worldDirty || m_localDirty;
    }

    void Transform::ForceRefreshMatrices()
    {
        m_localDirty = true;
        m_worldDirty = true;
        RefreshMatrices();
        for (auto& child : GetOwner()->GetChildren())
            if (child) child->GetTransform()->ForceRefreshMatrices();
    }

    JsonNode Transform::Serialize(JsonNode& parent) const
    {
        JsonNode transformObj = parent.CreateObjectField("transform");
        
        JsonNode node = transformObj.CreateObjectField("position");
        node.CreateField("x", m_localPosition.x);
        node.CreateField("y", m_localPosition.y);
        node.CreateField("z", m_localPosition.z);

        node = transformObj.CreateObjectField("rotation");
        node.CreateField("x", m_localRotation.x);
        node.CreateField("y", m_localRotation.y);
        node.CreateField("z", m_localRotation.z);
        node.CreateField("w", m_localRotation.w);

        node = transformObj.CreateObjectField("scale");
        node.CreateField("x", m_localScale.x);
        node.CreateField("y", m_localScale.y);
        node.CreateField("z", m_localScale.z);

        vec3 localEulerAngles = GetLocalEulerAngles();
        node = transformObj.CreateObjectField("euler_angles");
        node.CreateField("x", localEulerAngles.x);
        node.CreateField("y", localEulerAngles.y);
        node.CreateField("z", localEulerAngles.z);

        return transformObj;
    }

    void Transform::Deserialize(const JsonNode& data)
    {
        JsonNode node = data.Child("position");
        if (node.IsValid() && node.IsObject())
        {     
            m_localPosition.x = node.GetValue<float>("x", 0.0f).Result; 
            m_localPosition.y = node.GetValue<float>("y", 0.0f).Result;
            m_localPosition.z = node.GetValue<float>("z", 0.0f).Result;
        }

        node = data.Child("rotation");
        if (node.IsValid() && node.IsObject())
        {
            m_localRotation.x = node.GetValue<float>("x", 0.0f).Result;
            m_localRotation.y = node.GetValue<float>("y", 0.0f).Result;
            m_localRotation.z = node.GetValue<float>("z", 0.0f).Result;
        }

        node = data.Child("scale");
        if (node.IsValid() && node.IsObject())
        {
            m_localScale.x = node.GetValue<float>("x", 1.0f).Result;
            m_localScale.y = node.GetValue<float>("y", 1.0f).Result;
            m_localScale.z = node.GetValue<float>("z", 1.0f).Result;
        }

        node = data.Child("euler_angles");
        if (node.IsValid() && node.IsObject())
        {
            SetEulerAngles({node.GetValue<float>("x", 0.0f).Result,
                            node.GetValue<float>("y", 0.0f).Result,
                            node.GetValue<float>("z", 0.0f).Result });
        }
    }

    void Transform::RefreshMatrices() const
    {
        if (!IsDirty()) return;

        matrix4 localMat = translate(matrix4(1.0f), m_localPosition) * toMat4(m_localRotation) * scale(matrix4(1.0f), m_localScale);

        if (auto parent = GetOwner()->GetParent().lock())
        {
            Transform* transform = parent->GetTransform();
            transform->RefreshMatrices();
            m_localToWorld = transform->m_localToWorld * localMat;
        }
        else
        {
            m_localToWorld = localMat;
        }

        m_worldToLocal = inverse(m_localToWorld);

        m_localDirty = false;
        m_worldDirty = false;

        m_transformNotifier.Notify(TransformNotification::OnChanged);
    }
};