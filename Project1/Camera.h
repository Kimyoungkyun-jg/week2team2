#pragma once
#include "Transform.h"

class Camera
{
public:
	Camera()
	{
		transform.SetLocation(FVector(0.0f, 0.0f, -3.0f));
	}
	~Camera() {}

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

	const FVector& GetRotation() const { return transform.GetRotation(); }
	void SetRotation(const FVector& rot) { transform.SetRotation(rot); }

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
	float GetFOV() const { return fov; }

	FMatrix GetViewMatrix() const;
	FMatrix GetProjectionMatrix(float aspectRatio) const;

	void Update();

private:
	Transform transform;

	float fov = 60.0f;
	float NearZ = 0.1f;
	float FarZ = 1000.0f;
	float speed = 5.0f;
};

