#include "GameScene.h"
#include"../SceneManager.h"
#include"../../Object/Player/Player.h"
#include"../../Object/Stage/Stage.h"
#include"../../Object/Background/Background.h"

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
	Math::Vector3 cameraPos = {1.5f,1.5f,-7.0f };
	transMat = Math::Matrix::CreateTranslation(cameraPos + playerPos);
	m_camera->SetCameraMatrix(transMat);

	//被写界深度
	m_camera->SetFocus(5, 4, 20);
}

void GameScene::Init()
{
//カメラ
	m_camera = std::make_unique<KdCamera>();
//平行光(ディレクショナルライト)								      方向      色
	KdShaderManager::Instance().WorkAmbientController().SetDirLight({ 0,-1,-1 }, { 3,3,3 });
//環境光(アンビエントライト)
	//KdShaderManager::Instance().WorkAmbientController().SetAmbientLight({ 1,1,1,1 });
//フォグ(霧)
	//KdShaderManager::Instance().WorkAmbientController().SetFogEnable(true, false);
	////距離フォグ														霧の色     密度
	//KdShaderManager::Instance().WorkAmbientController().SetDistanceFog({ 0.3,0.3,0.3 }, 0.1f);
	////高さフォグ														霧の色     上 下 カメラとの距離
	//KdShaderManager::Instance().WorkAmbientController().SetheightFog({ 1,1,1 }, 0, -2, 0);



//プレイヤー
	std::shared_ptr<Player> player;
	player = std::make_shared<Player>();
	m_objList.push_back(player);
	m_player = player;

//ステージ
	std::shared_ptr<Stage> stage;
	stage = std::make_shared<Stage>();
	m_objList.push_back(stage);
//背景
	std::shared_ptr<Background> background;
	background = std::make_shared<Background>();
	m_objList.push_back(background);

}
