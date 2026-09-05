#include "pch.h"
#include "Camera.h"
#include "Global.h"
#include "Transform.h"


void Camera::Rotate(float deltaYaw, float deltaPitch)
{
	float deltaYawRad = deltaYaw * (Global::PI / 180.0f);
	float deltaPitchRad = deltaPitch * (Global::PI / 180.0f);

	FVector rot = transform.GetRotation();
	rot.y += deltaYawRad;
	rot.x += deltaPitchRad;

	// 짐벌락 방지 (-89도 ~ +89도)
	float maxPitch = 89.0f * (Global::PI / 180.0f);
	if (rot.x > maxPitch)
		rot.x = maxPitch;
	if (rot.x < -maxPitch)
		rot.x = -maxPitch;

	transform.SetRotation(rot);
}

DirectX::XMMATRIX Camera::GetViewMatrix() const
{
	const FVector& loc = transform.GetLocation();
	FVector fwd = transform.Forward();
	FVector up = transform.Up();

	DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(loc.x, loc.y, loc.z, 1.0f);
	DirectX::XMVECTOR focusPos = DirectX::XMVectorSet(loc.x + fwd.x, loc.y + fwd.y, loc.z + fwd.z, 1.0f);
	DirectX::XMVECTOR upDir = DirectX::XMVectorSet(up.x, up.y, up.z, 0.0f);

	return DirectX::XMMatrixLookAtLH(eyePos, focusPos, upDir);
}

DirectX::XMMATRIX Camera::GetProjectionMatrix(float aspectRatio) const
{
	float fovRadians = fov * (Global::PI / 180.0f);
	return DirectX::XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, NearZ, FarZ);
}

void Camera::Update()
{
	float speed = 5.0f * DELTA;
	if (KEY_PRESS(ImGuiKey_W)) MoveForward(speed);
	if (KEY_PRESS(ImGuiKey_S)) MoveForward(-speed);
	if (KEY_PRESS(ImGuiKey_D)) MoveRight(speed);
	if (KEY_PRESS(ImGuiKey_A)) MoveRight(-speed);
	if (KEY_PRESS(ImGuiKey_Q)) MoveUp(speed);
	if (KEY_PRESS(ImGuiKey_E)) MoveUp(-speed);


	if (MOUSE_PRESS(0)) {
		ImVec2 delta = ImGui::GetIO().MouseDelta;
		Rotate(delta.x * 0.2f, delta.y * 0.2f);
	}
}