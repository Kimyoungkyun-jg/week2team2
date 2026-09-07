#pragma once

class ASphere : public AActor
{
	// ASphere 부모 AActor -> ACollider로 변경
	DECLARE_CLASS(ASphere, ACollider);

public:
	ASphere() {
		InitVertexBuffer(sphere_vertices);
	}


};

