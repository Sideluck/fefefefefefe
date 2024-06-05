#pragma once

class Stage :public KdGameObject
{
public:
	Stage() { Init(); }
	~Stage() {}

	void Init()override;
	void GenerateDepthMapFromLight()override;
	void DrawLit()override;

private:
	Math::Matrix m_scaleMat;
	Math::Matrix m_rotMat;
	Math::Matrix m_transMat;

	std::shared_ptr<KdModelData> m_model;
};