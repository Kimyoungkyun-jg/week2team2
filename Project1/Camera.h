#pragma once
class Camera
{
public:
	Camera() {};
	~Camera() {};

	void Rotate(float deltaYaw, float deltaPitch);

	const FVector& GetLocation() const { return location; }

	float GetNear() const { return NearZ; }
	float GetFar() const { return FarZ; }
	float GetSpeed() const { return speed; }

	FVector GetForward() const;
	FVector GetRight() const;
	FVector GetUp() const;

	void SetLocation(const FVector& loc) { location = loc; }

	void MoveForward(float delta) { location += GetForward() * delta; }
	void MoveRight(float delta) { location += GetRight() * delta; }	

	DirectX::XMMATRIX GetViewMatrix() const;
	DirectX::XMMATRIX GetProjectionMatrix(float aspectRatio) const;

private:
	FVector location = FVector(0.0f, 0.0f, 0.0f);
	float yaw = 0.0f;
	float pitch = 0.0f;

	float fov = 60.0f;
	float NearZ = 0.1f;
	float FarZ = 1000.0f;
	float speed = 5.0f;
};

