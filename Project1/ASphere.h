#pragma once

class ASphere : public AActor
{
	// ASphere 부모 AActor -> ACollider로 변경
	DECLARE_CLASS(ASphere, ACollider);

public:
	ASphere() {
		Mesh* newMesh = new Mesh();
		newMesh->InitVertexBuffer(sphere_vertices);
		SetMesh(newMesh, true);
	}


};

