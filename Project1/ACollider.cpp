#include "pch.h"
#include "ACollider.h"
#include "Global.h"

void ACollider::Move(float deltaTime)
{
	if (Mass <= 0.0f || bSleeping)
	{
		return;
	}

	if (bUseGravity)
	{
		Velocity += Global::G * deltaTime;
	}

	Velocity = Velocity * (1.0f / (1.0f + deltaTime * LinearDamping));
	AngularVelocity *= 1.0f / (1.0f + deltaTime * AngularDamping);

	transform.Location += Velocity * deltaTime;
	transform.Rotation.z += AngularVelocity * deltaTime;
}

void ACollider::Pressed()
{
}

void ACollider::Released()
{
}

#include "Sphere.h"
#include "CircleGenerator.h"
#include "ObjectManager.h"

ACube::ACube(const FLinearColor& inColor)
	: ACollider(inColor)
{
	SetMesh(OBJECT.GetOrCreateMesh("Cube", cube_vertices));
	Primitive = EPrimitive::Cube;
}

ASphere::ASphere(const FLinearColor& inColor)
	: ACollider(inColor)
{
	SetMesh(OBJECT.GetOrCreateMesh("Sphere", sphere_vertices));
	Primitive = EPrimitive::Sphere;
}

ATriangle::ATriangle(const FLinearColor& inColor)
	: ACollider(inColor)
{
	SetMesh(OBJECT.GetOrCreateMesh("Triangle", triangle_vertices));
	Primitive = EPrimitive::Triangle;
}

ARectangle::ARectangle(const FLinearColor& inColor)
	: ACollider(inColor)
{
	SetMesh(OBJECT.GetOrCreateMesh("Rectangle", rectangle_vertices));
	Primitive = EPrimitive::Rectangle;
}

ACircle::ACircle(const FLinearColor& inColor)
	: ACollider(inColor)
{
	TArray<FVertexColor> circle_vertices = CircleGenerator::MakeCircle(32, 1.0f, 1.0f, 0.0f, 1.0f);
	SetMesh(OBJECT.GetOrCreateMesh("Circle", circle_vertices));
	Primitive = EPrimitive::Circle;
}


