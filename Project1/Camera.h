#pragma once
#include "Transform.h"

class CameraBuffer;

enum {
	Perspective,
	Orthographic,
};

class Camera
{
public:
	Camera();
	~Camera();

	static Camera& GetInstance() {
		static Camera instance;
		return instance;
	}
	Camera(const Camera&) = delete;
	Camera& operator=(const Camera&) = delete;

	Transform& GetTransform() { return transform; }
	const Transform& GetTransform() const { return transform; }
	void SetTransform(const Transform& inTransform) { transform = inTransform; }

	const FVector& GetLocation() const { return transform.GetLocation(); }
	void SetLocation(const FVector& loc) { transform.SetLocation(loc); }

	const FQuaternion& GetRotation() const { return transform.GetRotation(); }
	void SetRotation(const FQuaternion& rot) { transform.SetRotation(rot); }

	FVector GetForward() const { return transform.Forward(); }
	FVector GetRight() const { return transform.Right(); }
	FVector GetUp() const { return transform.Up(); }

	void Rotate(float deltaYaw, float deltaPitch);

	void MoveForward(float delta) { transform.SetLocation(transform.GetLocation() + GetForward() * delta); }
	void MoveRight(float delta) { transform.SetLocation(transform.GetLocation() + GetRight() * delta); }
	void MoveUp(float delta) { transform.SetLocation(transform.GetLocation() + GetUp() * delta); }

	float GetNear() const { return NearZ; }
	float GetFar() const { return FarZ; }
	float GetSpeed() const { return speed; }
	void SetSpeed(float inSpeed) { speed = inSpeed; }
	float& GetSpeedRef() { return speed; }

	float GetRotationSpeed() const { return rotationSpeed; }
	void SetRotationSpeed(float inRotSpeed) { rotationSpeed = inRotSpeed; }
	float& GetRotationSpeedRef() { return rotationSpeed; }

	float GetFOV() const { return fov; }

	FMatrix GetViewMatrix() const;
	FMatrix GetProjectionMatrix(float aspectRatio) const;

	void SetProjectionMode(int mode) { ProjectionMode = mode; }
	int GetProjectionMode() { return ProjectionMode; }

	void SetOrthoWidth(float w) { OrthoWidth = w; }
	float GetOrthWidth() { return OrthoWidth; }

	void Update();
	void SetVPBuffer();

	CameraBuffer* vpBuffer;
private:
	Transform transform;

	int ProjectionMode = Perspective;
	float OrthoWidth = 10.0f;

	float fov = 60.0f;
	float NearZ = 0.1f;
	float FarZ = 1000.0f;
	float speed = 2.0f;
	float rotationSpeed = 0.08f;
};

