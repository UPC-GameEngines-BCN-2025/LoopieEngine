#include "OrbitalCamera.h"
#include "Loopie/Core/Time.h"
#include "Loopie/Scene/Entity.h"
#include "Loopie/Components/Transform.h"
#include "Loopie/Components/MeshRenderer.h"
#include "Editor/Interfaces/Workspace/HierarchyInterface.h"
#include "Loopie/Core/Log.h"
#include "Loopie/Math/AABB.h"

namespace Loopie
{
	OrbitalCamera::OrbitalCamera()
	{
		m_entity = std::make_shared<Entity>("OrbitalCamera");
        m_entityToPivot = m_entity;
		m_entity->AddComponent<Transform>();
		m_camera = m_entity->AddComponent<Camera>( 45.0f,  0.1f, 1000.0f, false);
	}

	OrbitalCamera::~OrbitalCamera()
	{

	}

    void OrbitalCamera::ProcessEvent(const InputEventManager& inputEvent)
    {
        m_inputDirection = vec3(0);
        m_inputRotation = vec3(0);
        m_panDirection = vec3(0);
        m_zoomInput = 0;

        vec2 mouseScroll = inputEvent.GetScrollDelta();
        vec2 mouseDelta = inputEvent.GetMouseDelta();

        if (inputEvent.GetKeyStatus(SDL_SCANCODE_LALT) == KeyState::REPEAT && m_entity != m_entityToPivot)
        {
            if (inputEvent.GetMouseButtonStatus(0) == KeyState::REPEAT)
            {
                m_inputRotation = vec3(mouseDelta.x, mouseDelta.y, 0);
            }
            if (inputEvent.GetMouseButtonStatus(2) == KeyState::REPEAT)
            {
                m_orbitOffset.z += mouseDelta.y * m_cameraOrbitalZoomSpeed;
            }
        }
        else
        {
            if (inputEvent.GetMouseButtonStatus(1) == KeyState::REPEAT)
            {
                m_entityToPivot = m_entity;
                m_panDirection = vec3(mouseDelta.x, mouseDelta.y, 0);
            }
            if (inputEvent.GetMouseButtonStatus(2) == KeyState::REPEAT)
            {
                m_entityToPivot = m_entity;
                m_inputRotation = vec3(mouseDelta.x, mouseDelta.y, 0);

                if (inputEvent.GetKeyStatus(SDL_SCANCODE_LSHIFT) == KeyState::REPEAT)
                    m_speedMultiplier = 2.0f;
                else
                    m_speedMultiplier = 1.0f;  
            }
            if (inputEvent.GetKeyStatus(SDL_SCANCODE_W) == KeyState::REPEAT) m_inputDirection.z += m_cameraMovementSpeed;
            if (inputEvent.GetKeyStatus(SDL_SCANCODE_S) == KeyState::REPEAT) m_inputDirection.z -= m_cameraMovementSpeed;
            if (inputEvent.GetKeyStatus(SDL_SCANCODE_A) == KeyState::REPEAT) m_inputDirection.x += m_cameraMovementSpeed;
            if (inputEvent.GetKeyStatus(SDL_SCANCODE_D) == KeyState::REPEAT) m_inputDirection.x -= m_cameraMovementSpeed;
        }
        if (inputEvent.GetKeyStatus(SDL_SCANCODE_F) == KeyState::DOWN)
        {
            if (HierarchyInterface::s_SelectedEntity != nullptr)
            {
                m_entityToPivot = HierarchyInterface::s_SelectedEntity;
                MeshRenderer* renderer = HierarchyInterface::s_SelectedEntity->GetComponent<MeshRenderer>();
                if (renderer)
                {
                    vec3 objectScale = renderer->GetWorldAABB().GetSize();
                    float maxScaleValue = objectScale.x;
                    maxScaleValue = objectScale.y > maxScaleValue ? objectScale.y : maxScaleValue;
                    maxScaleValue = objectScale.z > maxScaleValue ? objectScale.z : maxScaleValue;
                    m_orbitOffset.z = -5 * maxScaleValue;
                }
            }
        }

        if (mouseScroll.y != 0)
            m_zoomInput = mouseScroll.y * m_cameraZoomSpeed;

        m_inputDirection *= m_speedMultiplier;
        m_inputRotation *= m_cameraRotationSpeed;
    }

    void OrbitalCamera::Update()
    {
        if (m_inputRotation == vec3{ 0.f, 0.f ,0.f} && m_inputDirection == vec3{ 0.f, 0.f, 0.f } && m_panDirection == vec3{ 0.f, 0.f, 0.f } && m_orbitOffset.z == 0.f)
            return;
        
        Transform* transform = m_entity->GetTransform();
 
        if (m_entityToPivot != m_entity)
        {
            m_yaw += -m_inputRotation.x;
            m_pitch += m_inputRotation.y;

            quaternion rotation = quaternion((vec3{ m_pitch, m_yaw, 0.f }));

            Transform* pivotTransform = m_entityToPivot->GetTransform();
            vec3 pivotPos = pivotTransform->GetPosition();

            vec3 camPos = transform->GetPosition();

            vec3 offset = rotation * m_orbitOffset;

            vec3 newPos = pivotPos + offset;
            transform->SetPosition(newPos);
            transform->SetRotation(rotation);

        }
        else
        {
            m_yaw += -m_inputRotation.x;
            m_pitch += m_inputRotation.y;

            m_inputDirection.x *= Time::GetDeltaTime();
            m_inputDirection.z *= Time::GetDeltaTime();

            m_inputDirection += m_panDirection/10.f;
            m_inputDirection.z += m_zoomInput;

            quaternion yawRotation = normalize(angleAxis(m_yaw, vec3(0, 1, 0)));
            quaternion pitchRotation = normalize(angleAxis(m_pitch, vec3(1, 0, 0)));
            quaternion orbitRotation = yawRotation * pitchRotation;

            transform->Translate(m_inputDirection, ObjectSpace::Local);
            transform->SetLocalRotation(orbitRotation);
        }
    }
}