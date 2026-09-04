#include "pch.h"
#include "Camera.h"
#include <Global.h>

void Camera::Rotate(float deltaYaw, float deltaPitch)
{
	yaw += deltaYaw;
	pitch += deltaPitch;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	// -----------------------------------------
	// ----------- 짐벌락 방지 코드 ------------
	// -----------------------------------------
}

FVector Camera::GetForward() const
{
	float yawRad = yaw * (Global::PI / 180.0f);
	float pitchRad = pitch * (Global::PI / 180.0f);

	FVector forward;
	forward.x = cos(pitchRad) * cos(yawRad);
	forward.y = cos(pitchRad) * sin(yawRad);
	forward.z = sin(pitchRad);
	forward.Normalize();
	return forward;
}

FVector Camera::GetRight() const
{
	FVector worldUp(0.0f, 0.0f, 1.0f);
	FVector right = FVector::Cross3D(GetForward(), worldUp);
	right.Normalize();
	return right;
}

FVector Camera::GetUp() const
{
	FVector up = FVector::Cross3D(GetRight(), GetForward());
	up.Normalize();
	return up;
}

DirectX::XMMATRIX Camera::GetViewMatrix() const
{
	DirectX::XMVECTOR eyePos = DirectX::XMVectorSet(location.x, location.y, location.z, 1.0f);
	DirectX::XMVECTOR focusPos = DirectX::XMVectorSet(location.x + GetForward().x, location.y + GetForward().y, location.z + GetForward().z, 1.0f);
	DirectX::XMVECTOR upDir = DirectX::XMVectorSet(GetUp().x, GetUp().y, GetUp().z, 0.0f);

	return DirectX::XMMatrixLookAtLH(eyePos, focusPos, upDir);
}

DirectX::XMMATRIX Camera::GetProjectionMatrix(float aspectRatio) const
{
	float fovRadians = fov * (Global::PI / 180.0f);
	return DirectX::XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, NearZ, FarZ);
}
  