#include "Player.h"
#include "../../Scene/SceneManager.h"

void Player::Update()
{
	UINT oldDirType = m_dirType;   //前回の方向タイプを退避
	m_dirType = 0;				   //ビット列をクリア

	//移動処理
	if (GetAsyncKeyState(VK_LEFT) & 0x8000)
	{
		m_pos.x -= 0.15f;
		m_scale.x = -2.0f;
		m_dirType |= DirType::Move;
	}
	else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
	{
		m_pos.x += 0.15f;
		m_scale.x = 2.0f;
		m_dirType |= DirType::Move;
	}
	else if (GetAsyncKeyState(VK_UP) & 0x8000)
	{
		m_pos.z += 0.1f;
		m_dirType |= DirType::Move;
	}
	else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
	{
		m_pos.z -= 0.1f;
		m_dirType |= DirType::Move;
	}
	else
	{
		m_dirType |= DirType::Idle;
	}

	//ジャンプ
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
	{
		if (!keyFlg &&jumpCount < maxJumpCount)
		{
			keyFlg = true;
			m_gravity = -0.2f;
			jumpFlg = true;
			jumpCount++;	
		}
	}
	else
	{
		keyFlg = false;
	}

	if (jumpFlg && wallKickCount < 1)
	{
		m_dirType |= DirType::Jump;
	}
	if (wallKickCount == 1)
	{
		m_dirType |= DirType::Attack;
	}

	//何かアクションをしたらアニメーション情報更新
	if (m_dirType != 0 && m_dirType != oldDirType)
	{
		ChangeAnimetion();
	}
	//変わっていないなら元の向き(退避データ)に戻す
	else
	{
		m_dirType = oldDirType;
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
	m_mWorld   = m_scaleMat * m_transMat;
}

void Player::PostUpdate()
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
	ray.m_pos.y += 0.5f;

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
		m_pos = hitPos + Math::Vector3(0, -0.5f, 0);
		m_gravity = 0;
		jumpCount = 0;
		wallKickCount = 0;
		jumpFlg = false;
	}
	else
	{
		if (!jumpFlg && jumpCount < 1)
		{
			jumpCount += 1;
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

		//壁キック
		if (jumpFlg&& !keyFlg&&IsKeyPressed(VK_SPACE) && wallKickCount < maxWallKickCount)
		{
			jumpCount = 1;
			wallKickCount++;
		}
	}
}

void Player::Init()
{
	m_polygon.SetMaterial("Asset/Textures/Ninja.png");
	m_pos   = { -10,-1,0 };
	m_scale = { 2,2,2 };

	//画像分割
	m_polygon.SetSplit(12, 6);

	//原点変更
	m_polygon.SetPivot(KdSquarePolygon::PivotType::Center_Bottom);

	//デバッグ用
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_animeInfo.start = 0;     //開始コマ
	m_animeInfo.end   = 12;    //終了コマ
	m_animeInfo.count = 0;     //現在のコマ数カウント
	m_animeInfo.speed = 0.2f;  //アニメーションの速度

	//向いている方向
	m_dirType = DirType::Idle;

	jumpFlg = false;

	keyFlg = false;

}

void Player::GenerateDepthMapFromLight()
{
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(m_polygon, m_mWorld);
}

void Player::DrawLit()
{
	KdShaderManager::Instance().m_StandardShader.DrawPolygon(m_polygon, m_mWorld);
}

void Player::WallKick(const Math::Vector3& hitDir)
{
	wallKickForce = 0.05f;

	m_pos += hitDir * wallKickForce;
	m_pos.y += wallKickForce; 
}

bool Player::IsKeyPressed(int key)
{
	return (GetAsyncKeyState(key) & 0x8000) != 0;
}

void Player::ChangeAnimetion()
{
	if (m_dirType & DirType::Idle)
	{
		m_animeInfo.start = 0;
		m_animeInfo.end = 11;
	}
	if (m_dirType & DirType::Move)
	{
		m_animeInfo.start = 12;
		m_animeInfo.end = 19;
	}
	if (m_dirType & DirType::Jump)
	{
		m_animeInfo.start = 24;
		m_animeInfo.end = 27;
	}
	if (m_dirType & DirType::Attack)
	{
		m_animeInfo.start = 38;
		m_animeInfo.end = 41;
	}

	//カウントとスピードを初期化
	m_animeInfo.count = 0;
	m_animeInfo.speed = 0.3f;
}
