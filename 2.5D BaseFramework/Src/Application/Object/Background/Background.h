#pragma once

class Background :public KdGameObject
{
public:
	Background() { Init(); }
	~Background() {}

	void Init()override;
	void DrawUnLit()override;

private:
	Math::Matrix m_scaleMat;
	Math::Matrix m_transMat;

	std::shared_ptr<KdModelData> m_model;
	KdSquarePolygon m_polygon;
};
