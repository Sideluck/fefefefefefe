#pragma once

#include"../BaseScene/BaseScene.h"

class Ninja;

class GameScene : public BaseScene
{
public :

	GameScene() { Init(); }
	~GameScene() {}

private:

	void Event() override;
	void Init() override;

	std::weak_ptr<Ninja> m_player;
};
