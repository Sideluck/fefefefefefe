#include "Ninja.h"
#include "../../Scene/SceneManager.h"

void Ninja::Update()
{
	UINT oldCurrentState = m_currentState;   //前回の状態を退避
	m_currentState = 0;				   //ビット列をクリア

	if (m_hp < 0)
	{
		m_currentState |= Currentstate::Death;
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		m_hp -= 3;
	}

	//移動処理
	if (m_hp > 0)
	{
		if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			m_pos.x -= 0.15f;
			m_scale.x = -2.0f;
			m_currentState |= Currentstate::Move;
		}
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			m_pos.x += 0.15f;
			m_scale.x = 2.0f;
			m_currentState |= Currentstate::Move;
		}
		else
		{
			m_currentState |= Currentstate::Idle;
		}

		//ジャンプ
		if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		{
			if (!m_keyFlg && m_jumpCount < m_maxJumpCount)
			{
				m_keyFlg = true;
				m_gravity = -0.2f;
				m_jumpFlg = true;
				m_jumpCount++;
			}
		}
		else
		{
			m_keyFlg = false;
		}

		if (m_jumpFlg && m_wallKickCount < 1 || m_airFlg)
		{
			m_currentState |= Currentstate::Jump;
		}
		if (m_wallMountedFlg && m_jumpFlg)
		{
			m_currentState |= Currentstate::WallMounted;
		}
	}

	//何かアクションをしたらアニメーション情報更新
	if (m_currentState != 0 && m_currentState != oldCurrentState)
	{
		ChangeAnimetion();
	}
	//変わっていないなら元の状態(退避データ)に戻す
	else
	{
		m_currentState = oldCurrentState;
	}

	if (m_pos.y < -20)
	{
		m_gravity = 0;
		m_pos = { 0,5,0 };
	}

	//重力をキャラに反映
	m_pos.y -= m_gravity;
	m_gravity += 0.008f;

	//アニメーション更新
	m_animeInfo.count += m_animeInfo.speed;
	int animeCnt = m_animeInfo.count + m_animeInfo.start;

	//最後のコマまで表示し終えたらループさせる
	if (animeCnt > m_animeInfo.end)
	{
		animeCnt = m_animeInfo.start;
		m_animeInfo.count = 0;
	}

	m_polygon.SetUVRect(animeCnt);

	//座標更新
	m_scaleMat = Math::Matrix::CreateScale(m_scale);
	m_transMat = Math::Matrix::CreateTranslation(m_pos);
	m_mWorld = m_scaleMat * m_transMat;
}

void Ninja::PostUpdate()
{
	//☆☆☆☆☆☆☆☆☆☆☆☆
	//☆当たり判定(レイ判定)☆
	//☆☆☆☆☆☆☆☆☆☆☆☆

	//レイ判定用の変数を作成
	KdCollider::RayInfo ray;
	//レイの発射位置(座標)を設定
	ray.m_pos = m_pos;
	//レイの発射方向を設定
	ray.m_dir = Math::Vector3::Down;//真下

	//少し高いところから飛ばす
	ray.m_pos.y += 0.4f;

	//段差の許容範囲を設定
	float enableStepHigh = 0.2f;
	ray.m_pos.y += enableStepHigh;

	//レイの長さを設定
	ray.m_range = m_gravity + enableStepHigh;

	//当たり判定をしたい対応を設定
	ray.m_type = KdCollider::TypeGround;

	//レイに当たったオブジェクト情報を作成
	std::list<KdCollider::CollisionResult> retRayList;

	//☆☆☆☆☆☆☆☆☆
	//レイと当たり判定☆
	//☆☆☆☆☆☆☆☆☆
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		obj->Intersects(ray, &retRayList);
	}

	//デバッグ用(レイを可視化)
	m_pDebugWire->AddDebugLine(ray.m_pos, ray.m_dir, ray.m_range);

	//レイリストから一番近いオブジェクトを検出
	float maxOverLap = 0;   //はみ出たレイの長さ
	Math::Vector3 hitPos;   //当たっていたらtrue
	bool ishit = false;

	for (auto& ret : retRayList)
	{
		if (maxOverLap < ret.m_overlapDistance)
		{
			maxOverLap = ret.m_overlapDistance;
			hitPos = ret.m_hitPos;
			ishit = true;
		}
	}
	if (ishit)
	{
		//当たっている
		m_pos = hitPos + Math::Vector3(0, -0.4f, 0);
		m_gravity = 0;
		m_jumpCount = 0;
		m_wallKickCount = 0;
		m_jumpFlg = false;
		m_airFlg = false;
	}
	else
	{
		m_airFlg = true;
		if (!m_jumpFlg && m_jumpCount < 1)
		{
			m_jumpCount += 1;
		}
	}

	//☆☆☆
	//球判定
	//☆☆☆

	//球判定用の変数を作成
	KdCollider::SphereInfo sphere;
	//球の中心位置を設定
	sphere.m_sphere.Center = m_pos + Math::Vector3(0, 0.9f, 0);
	//球の半径を設定
	sphere.m_sphere.Radius = 0.35f;
	//当たり判定をしたいタイプを設定
	sphere.m_type = KdCollider::TypeGround;
	//デバッグ用
	m_pDebugWire->AddDebugSphere(sphere.m_sphere.Center, sphere.m_sphere.Radius);

	//球が当たったオブジェクト情報を作成
	std::list<KdCollider::CollisionResult> retSphereList;

	//☆☆☆☆☆☆☆☆☆
	//球と当たり判定☆
	//☆☆☆☆☆☆☆☆☆
	for (auto& obj : SceneManager::Instance().GetObjList())
	{
		obj->Intersects(sphere, &retSphereList);
	}

	//球リストから一番近いオブジェクトを検出
	maxOverLap = 0;   //はみ出た球の長さ
	ishit = false;
	Math::Vector3 hitDir; //当たった方向

	for (auto& ret : retSphereList)
	{
		if (maxOverLap < ret.m_overlapDistance)
		{
			maxOverLap = ret.m_overlapDistance;
			hitDir = ret.m_hitDir;
			ishit = true;
		}
	}
	if (ishit)
	{
		//Z方向無効
		hitDir.z = 0;
		//方向ベクトルは長さ1にする必要がある
		hitDir.Normalize();

		//押し戻し
		m_pos += hitDir * maxOverLap;

		//壁張り付き
		if (m_hp > 0)
		{
			if (m_jumpFlg && !m_keyFlg && m_wallKickCount < m_maxWallKickCount)
			{
				if (GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState(VK_RIGHT) & 0x8000)
				{
					m_gravity = 0.00f;
					if (GetAsyncKeyState(VK_UP) & 0x8000)
					{
						m_pos.y += 0.02f;
						m_wallMountedFlg = true;
					}
					else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
					{
						m_pos.y -= 0.02f;
						m_wallMountedFlg = true;
					}
					else
					{
						m_wallMountedFlg = false;
					}

					if (GetAsyncKeyState(VK_SPACE) & 0x8000)
					{
						m_jumpCount = 1;
						m_wallKickCount++;
					}
				}
			}
			else
			{
				m_wallMountedFlg = false;
			}
		}
		else
		{
			m_wallMountedFlg = false;

		}
	}
}
	

void Ninja::Init()
{
	m_polygon.SetMaterial("Asset/Textures/Ninja.png");
	m_pos = { -43,-1.5,-27 };
	m_scale = { 2,2,2 };

	//画像分割
	m_polygon.SetSplit(8, 4);

	//原点変更
	m_polygon.SetPivot(KdSquarePolygon::PivotType::Center_Bottom);

	//デバッグ用
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_animeInfo.start     = 0;	   //開始コマ
	m_animeInfo.end       = 3;	   //終了コマ
	m_animeInfo.count     = 0;     //現在のコマ数カウント
	m_animeInfo.speed     = 0.15f; //アニメーションの速度

	m_MakimonoTex = std::make_shared<KdTexture>();
	m_MakimonoTex->Load("Asset/Textures/Makimono.png");

	//向いている方向
	m_currentState = Currentstate::Idle;

	m_jumpFlg = false;

	m_keyFlg = false;

	m_hp = 3;

}

void Ninja::GenerateDepthMapFromLight()
{
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(m_polygon, m_mWorld);
}

void Ninja::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(m_polygon, m_mWorld);
}

void Ninja::DrawSprite()
{
	Math::Rectangle ninjaIconRect = { 0,0,177,81 };
	Math::Color color = { 1,1,1,1 };
	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::CreateScale(2));
	KdShaderManager::Instance().m_spriteShader.DrawTex(m_MakimonoTex, -220,130, &ninjaIconRect, &color);
	KdShaderManager::Instance().m_spriteShader.SetMatrix(Math::Matrix::Identity);
}

void Ninja::ChangeAnimetion()
{
	if (m_currentState & Currentstate::Idle)
	{
		m_animeInfo.start = 0;
		m_animeInfo.end = 3;
		m_animeInfo.count = 0;
		m_animeInfo.speed = 0.1f;
	}
	if (m_currentState & Currentstate::Move)
	{
		m_animeInfo.start = 8;
		m_animeInfo.end = 11;
		m_animeInfo.count = 0;
		m_animeInfo.speed = 0.15f;
	}
	if (m_currentState & Currentstate::Jump)
	{
		m_animeInfo.start = 16;
		m_animeInfo.end = 16;
		m_animeInfo.count = 0;
		m_animeInfo.speed = 0.0f;
	}
	if (m_currentState & Currentstate::Attack)
	{
		m_animeInfo.start = 24;
		m_animeInfo.end = 27;
		m_animeInfo.count = 0;
		m_animeInfo.speed = 0.15f;
	}
	if (m_currentState & Currentstate::WallMounted)
	{
		m_animeInfo.start = 12;
		m_animeInfo.end = 15;
		m_animeInfo.count = 0;
		m_animeInfo.speed = 0.15f;
	}
	if (m_currentState & Currentstate::Death)
	{
		m_animeInfo.start = 7;
		m_animeInfo.end = 7;
		m_animeInfo.count = 0;
		m_animeInfo.speed = 0.00f;
	}
}
