#include "Player.h"
#include "../../Scene/SceneManager.h"

void Player::Update()
{
	m_dirType = 0;	//ビット列をクリア

	//移動処理
	if (IsKeyPressed(VK_LEFT))
	{
		m_pos.x -= m_posPow;
		m_scale.x = -1.5f;
		m_dirType = DirType::Move;
	}
	else if (IsKeyPressed(VK_RIGHT))
	{
		m_pos.x += m_posPow;
		m_scale.x = 1.5f;
		m_dirType = DirType::Move;
	}
	else
	{
		m_dirType = DirType::Idle;
	}

	//ジャンプ
	if (IsKeyPressed(VK_SPACE))
	{
		if (!keyFlg && jumpCount < maxJumpCount)
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

	if (jumpFlg && wallKickCount < 1||airFlg)
	{
		m_dirType = DirType::Jump;
	}
	if (wallKickCount == 1)
	{
		m_dirType = DirType::Attack;
	}


	if (m_dirType = Idle)
	{
		int Idle[4] = { 0,7,14,21 };
		m_polygon.SetUVRect(Idle[(int)m_anim]);
		m_anim += 0.1f;
		if (m_anim >= 4)
		{
			m_anim = 0;
		}
	}
	else if (m_dirType = Move)
	{
		int Move[4] = { 1,8,15,22 };
		m_polygon.SetUVRect(Move[(int)m_anim]);
		m_anim += 0.1f;
		if (m_anim >= 4)
		{
			m_anim = 0;
		}
	}

	ChangeAnimetion();

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
	//当たり判定(レイ判定)

	//レイ判定用の変数を作成
	KdCollider::RayInfo ray;
	//レイの発射位置(座標)を設定
	ray.m_pos = m_pos;
	//レイの発射方向を設定
	ray.m_dir = Math::Vector3::Down;//真下

	//少し高いところから飛ばす
	ray.m_pos.y += 0.3f;

	//段差の許容範囲を設定
	float enableStepHigh = 0.2f;
	ray.m_pos.y += enableStepHigh;

	//レイの長さを設定
	ray.m_range = m_gravity + enableStepHigh;

	//当たり判定をしたい対応を設定
	ray.m_type = KdCollider::TypeGround;

	//レイに当たったオブジェクト情報を作成
	std::list<KdCollider::CollisionResult> retRayList;

	//レイと当たり判定
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
		m_pos = hitPos + Math::Vector3(0, -0.3f, 0);
		m_gravity = 0;
		jumpCount = 0;
		wallKickCount = 0;
		jumpFlg = false;
		airFlg = false;
	}
	else
	{
		airFlg = true;
		if (!jumpFlg && jumpCount < 1)
		{
			jumpCount += 1;
		}
	}

	//球判定

	//球判定用の変数を作成
	KdCollider::SphereInfo sphere;
	//球の中心位置を設定
	sphere.m_sphere.Center = m_pos + Math::Vector3(0, 0.7f, 0);
	//球の半径を設定
	sphere.m_sphere.Radius = 0.35f;
	//当たり判定をしたいタイプを設定
	sphere.m_type = KdCollider::TypeGround;
	//デバッグ用
	m_pDebugWire->AddDebugSphere(sphere.m_sphere.Center, sphere.m_sphere.Radius);

	//球が当たったオブジェクト情報を作成
	std::list<KdCollider::CollisionResult> retSphereList;

	//球と当たり判定

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
	//m_polygon.SetMaterial("Asset/Textures/Ninja.png");
	m_polygon.SetMaterial("Asset/Textures/ninja-black.png");
	m_pos   = { -10,-1,0 };
	m_scale = { 1.5,1.5,1.5 };

	//画像分割
	m_polygon.SetSplit(7, 4);

	//原点変更
	m_polygon.SetPivot(KdSquarePolygon::PivotType::Center_Bottom);

	//デバッグ用
	m_pDebugWire = std::make_unique<KdDebugWireFrame>();

	m_animeInfo.start = 0;     //開始コマ
	m_animeInfo.end   = 11;    //終了コマ
	m_animeInfo.count = 0;     //現在のコマ数カウント
	m_animeInfo.speed = 0.3f;  //アニメーションの速度

	//向いている方向
	m_dirType = DirType::Idle;

	m_posPow = 0.15;

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

bool Player::IsKeyPressed(int key)
{
	return (GetAsyncKeyState(key) & 0x8000) != 0;
}

void Player::ChangeAnimetion()
{
	/*if (m_dirType & DirType::Idle)
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
	}*/

	switch (m_dirType)
	{
	case DirType::Idle:
		m_animeInfo.start = 0;
		m_animeInfo.end = 3;
		break;
	case DirType::Move:
		m_animeInfo.start = 4;
		m_animeInfo.end = 7;
		break;
	case DirType::Jump:
		m_animeInfo.start = 7;
		m_animeInfo.end = 8;
		break;
	case DirType::Attack:
		m_animeInfo.start = 6;
		m_animeInfo.end = 1;
		break;
	default:
		break;
	}

	
}
