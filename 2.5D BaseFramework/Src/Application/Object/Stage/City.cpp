#include "City.h"

void City::Init()
{
	//データ読みこみ
	m_model = std::make_shared<KdModelData>();
	m_model->Load("Asset/Models/yard/yard.gltf");

	m_transMat = Math::Matrix::CreateTranslation(0, -2.0f, -5);
	m_rotMat   = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(90));
	m_scaleMat = Math::Matrix::CreateScale(5, 5, 5);

	//ワールド行列は絶対使う!(自分で作った行列は使わない)
	m_mWorld = m_scaleMat* m_rotMat *m_transMat;

	m_pCollider = std::make_unique<KdCollider>();
	m_pCollider->RegisterCollisionShape("StageCollison", m_model, KdCollider::TypeGround);
}

void City::GenerateDepthMapFromLight()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}

void City::DrawLit()
{
	//表示
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
