#include "pch.h"
#include "Camera.h"
#include "Global.h"
#include "Transform.h"


Camera::Camera()
{
	//기본 카메라 위치 및 회전 설정
	transform.SetLocation(FVector(3.336f, 3.282f, -4.715f));
	transform.SetRotation(FVector(0.391f, -0.468f, 0.0f));

	vpBuffer = new CameraBuffer();
}


Camera::~Camera()
{
	delete vpBuffer;
}

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
	//카메라 이동 처리
	float currentSpeed = speed * DELTA;
	if (KEY_PRESS(ImGuiKey_W)) MoveForward(currentSpeed);
	if (KEY_PRESS(ImGuiKey_S)) MoveForward(-currentSpeed);
	if (KEY_PRESS(ImGuiKey_D)) MoveRight(currentSpeed);
	if (KEY_PRESS(ImGuiKey_A)) MoveRight(-currentSpeed);
	if (KEY_PRESS(ImGuiKey_Q)) MoveUp(currentSpeed);
	if (KEY_PRESS(ImGuiKey_E)) MoveUp(-currentSpeed);

	//카메라 회전 처리
	if (MOUSE_PRESS(1)) {
		ImVec2 delta = ImGui::GetIO().MouseDelta;
		Rotate(delta.x * 0.2f, delta.y * 0.2f);
	}
}

void Camera::SetVPBuffer()
{
	vpBuffer->SetVSBuffer(1);
}


