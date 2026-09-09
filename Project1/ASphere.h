#pragma once

class ASphere : public AActor
{
	// ASphere 부모 AActor -> ACollider로 변경
	DECLARE_CLASS(ASphere, ACollider);

public:
	ASphere() {
		// 오브젝트 매니저를 통해 구 메시 캐시 사용
		SetMesh(OBJECT.GetOrCreateMesh("Sphere", sphere_vertices));
	}


};

