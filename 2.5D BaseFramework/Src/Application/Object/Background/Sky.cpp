#include "Sky.h"

void Sky::Init()
{
	/*m_polygon.SetMaterial("Asset/Textures/BackGround.png");
	m_scaleMat = Math::Matrix::CreateScale(300, 10, 1);
	m_transMat = Math::Matrix::CreateTranslation(0, 25, 100);*/

	m_model = std::make_shared<KdModelData>();
	m_model->Load("Asset/Models/sky/sky.gltf");
	m_scaleMat = Math::Matrix::CreateScale(150, 150, 150);
	m_transMat = Math::Matrix::CreateTranslation(0, 0, 0);

	m_mWorld = m_scaleMat * m_transMat;

}

void Sky::DrawUnLit()
{
	//KdShaderManager::Instance().m_StandardShader.DrawPolygon(m_polygon, m_mWorld);
	KdShaderManager::Instance().m_StandardShader.DrawModel(*m_model, m_mWorld);
}
