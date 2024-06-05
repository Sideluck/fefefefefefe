#include "Stage.h"

void Stage::Init()
{
	//データ読みこみ
	m_model = std::make_shared<KdModelData>();
	m_model->Load("Asset/Models/city/city.gltf");

	m_transMat = Math::Matrix::CreateTranslation(0, -2.0f, -5);
	m_rotMat   = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(90));
	m_scaleMat = Math::Matrix::CreateScale(5, 5, 5);

	//ワールド行列は絶対使う!(自分で作った行列は使わない)
	m_mWorld = m_scaleMat *m_transMat;

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("StageCollison", m_model, KdCollider::TypeGround);
}

void Stage::GenerateDepthMapFromLight()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void Stage::DrawLit()
{
	//表示
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
