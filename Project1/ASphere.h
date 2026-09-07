#pragma once

class ASphere : public AActor
{
	DECLARE_CLASS(ASphere, AActor);

public:
	ASphere() {
		InitVertexBuffer(sphere_vertices);
	}


};

