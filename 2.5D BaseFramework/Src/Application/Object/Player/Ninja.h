#pragma once
class Ninja :public KdGameObject
{
public:

	enum Currentstate
	{
		Idle		= 1 << 0,	//立ち姿
		Move		= 1 << 1,	//移動
		Jump		= 1 << 2,	//ジャンプ
		Attack		= 1 << 3,	//攻撃
		WallMounted = 1 << 4,	//壁張り付き
		Death       = 1 << 5,	//やられた
	};

	struct AnimationInfo
	{
		int start;			//開始コマ
		int end;			//終了コマ
		float count;		//現在のコマ数カウント
		float speed;		//アニメーションの速度
	};

	Ninja() { Init(); }
	~Ninja() {}

	void Update()override;
	void PostUpdate()override;
	void Init()override;
	void GenerateDepthMapFromLight()override;
	void DrawLit()override;
	void DrawSprite()override;


private:

	std::shared_ptr<KdTexture> m_MakimonoTex;

	Math::Matrix m_scaleMat;
	Math::Matrix m_rotMat;
	Math::Matrix m_transMat;

	KdSquarePolygon m_polygon;
	Math::Vector3 m_pos;
	Math::Vector3 m_scale;

	//アニメーション
	float m_anim = 0;

	void ChangeAnimetion();

	//キャラが向いている方向種類 ビット管理
	UINT m_currentState;

	AnimationInfo m_animeInfo;

	//ジャンプしているかのフラグ
	bool m_jumpFlg = false;

	//空中にいるかのフラグ
	bool m_airFlg = false;

	//壁に張り付いているかのフラグ
	bool m_wallMountedFlg = false;
	bool m_wallMountedMoveFlg = false;

	//重力
	float m_gravity = 0;

	//移動量
	float m_posPow = 0;

	//キー制御するフラグ
	bool m_keyFlg = false;		

	//ジャンプカウント
	int m_jumpCount = 0;			

	//ジャンプの最大数
	const int m_maxJumpCount = 2; 

	//壁ジャンプカウント
	int m_wallKickCount = 0;			

	//壁ジャンプの最大数
	const int m_maxWallKickCount = 1; 

	//体力
	int m_hp = 0;

};