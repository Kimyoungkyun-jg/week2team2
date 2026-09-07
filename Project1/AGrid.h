#pragma once
class AGrid : public AActor
{
	DECLARE_CLASS(AGrid, AActor)

public:

	AGrid(EGridType InType = EGridType::Line);

	void CreateVertices();
	void CreateLineVertices();
	void CreateTriangleVertices();

	vector<FVertexColor> vertices;

	static constexpr int MAX_VALUE = 100;

	UINT width = MAX_VALUE;
	UINT height = MAX_VALUE;
	EGridType GridType = EGridType::Line;

	virtual void Render() override;

};
