#include "Sky.h"

void Sky::Init()
{
	m_model = std::make_shared<KdModelData>();
	m_model->Load("Asset/Models/sky/sky.gltf");
}

void Sky::Update()
{
	m_rotY += 0.01f;
	if (m_rotY > 360)
	{
		m_rotY -= 360;
	}
	m_scaleMat = Math::Matrix::CreateScale(150, 150, 150);
	m_rotMat = Math::Matrix::CreateRotationY(DirectX::XMConvertToRadians(m_rotY));
	m_transMat = Math::Matrix::CreateTranslation(0, 0, 0);

	m_mWorld = m_scaleMat * m_rotMat * m_transMat;
}

void Sky::DrawUnLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
