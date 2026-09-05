#include "pch.h"
#include "Camera.h"
#include "Global.h"

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

FMatrix Camera::GetViewMatrix() const
{
	const FVector& eye = transform.GetLocation();
	FVector target = eye + transform.Forward();
	FVector up = transform.Up();

	return FMatrix::LookAt(eye, target, up);
}

FMatrix Camera::GetProjectionMatrix(float aspectRatio) const
{
	float fovRadians = fov * (Global::PI / 180.0f);
	return FMatrix::PerspectiveFov(fovRadians, aspectRatio, NearZ, FarZ);
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