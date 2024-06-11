#pragma once
class Player :public KdGameObject
{
public:

	enum DirType
	{
		Idle   = 1 << 0,	//立ち姿
		Move   = 1 << 1,	//移動
		Jump   = 1 << 2,	//ジャンプ
		Attack = 1 << 3,	//攻撃
	};

	struct AnimationInfo
	{
		int start;		//開始コマ
		int end;		//終了コマ
		float count;    //現在のコマ数カウント
		float speed;	//アニメーションの速度
	};

	Player() { Init(); }
	~Player() {}

	void Update()override;
	void PostUpdate()override;
	void Init()override;
	void GenerateDepthMapFromLight()override;
	void DrawLit()override;

	void WallKick(const Math::Vector3& hitDir);
	bool IsKeyPressed(int key);

private:

	Math::Matrix m_scaleMat;
	Math::Matrix m_rotMat;
	Math::Matrix m_transMat;

	KdSquarePolygon m_polygon;
	Math::Vector3 m_pos;
	Math::Vector3 m_scale;

	Math::Vector3 m_velocity;
	float wallKickForce = 0;

	void ChangeAnimetion();

	//キャラが向いている方向種類 ビット管理
	UINT m_dirType;

	AnimationInfo m_animeInfo;

	//ジャンプしているかのフラグ
	bool jumpFlg = false;

	//重力
	float m_gravity = 0;

	bool keyFlg = false;
	int jumpCount = 0;			//ジャンプカウント
	const int maxJumpCount = 2; //何回連続でジャンプできるかのカウント
	int wallKickCount = 0;			//ジャンプカウント
	const int maxWallKickCount = 1;
};