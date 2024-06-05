#include "GameScene.h"
#include"../SceneManager.h"
#include"../../Object/Player/Player.h"
#include"../../Object/Stage/Stage.h"

void GameScene::Event()
{
	if (GetAsyncKeyState('T') & 0x8000)
	{
		SceneManager::Instance().SetNextScene
		(
			SceneManager::SceneType::Title
		);
	}
	Math::Vector3 playerPos;
	if (m_player.expired() == false)
	{
		playerPos = m_player.lock()->GetPos();
	}

	Math::Matrix transMat;
	Math::Vector3 cameraPos = { 0,1.5f,-5.0f };
	transMat = Math::Matrix::CreateTranslation(cameraPos + playerPos);
	m_camera->SetCameraMatrix(transMat);
}

void GameScene::Init()
{
//カメラ
	m_camera = std::make_unique<KdCamera>();

//プレイヤー
	std::shared_ptr<Player> player;
	player = std::make_shared<Player>();
	m_objList.push_back(player);
	m_player = player;

//ステージ
	std::shared_ptr<Stage> stage;
	stage = std::make_shared<Stage>();
	m_objList.push_back(stage);
}
