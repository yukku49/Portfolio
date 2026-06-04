#include "PlayerManagiment.h"
#include "ItemManagiment.h"
#include "BackScreenManagiment.h"
#include "BllentManagiment.h"

#include "DxLib.h"
#include <cmath>


static constexpr int TILE_SIZE = 32;

void Player_Managiment::Player_BringItem(Item_number picked)
{
	switch (picked)
	{
	case (TOMATO): Player_Itembring.Tmato_Counter++; break;
	case (BASIL): Player_Itembring.Basil_Counter++; break;
	case (CHEESE): Player_Itembring.Cheese_Counter++; break;
	case (GORGONZOLA): Player_Itembring.Gorgonzola_Counter++; break;
	case (PIZZADOUGH): Player_Itembring.Pizzadough_Counter++; break;
	}
}

PizzaType Player_Managiment::TryMakePizza()
{
	auto& it = Player_Itembring;

	// マルゲリータ（最優先）
	if (it.Tmato_Counter > 0 && it.Cheese_Counter > 0 &&
		it.Basil_Counter > 0 && it.Pizzadough_Counter > 0)
	{
		it.Tmato_Counter--;
		it.Cheese_Counter--;
		it.Basil_Counter--;
		it.Pizzadough_Counter--;
		return PizzaType::Margherita;
	}
	// クアトロフォルマッジ
	if (it.Tmato_Counter > 0 && it.Cheese_Counter > 0 &&
		it.Pizzadough_Counter > 0)
	{
		it.Tmato_Counter--;
		it.Cheese_Counter--;
		it.Pizzadough_Counter--;
		return PizzaType::QuattroFormaggi;
	}
	// マリナーラ
	if (it.Tmato_Counter > 0 && it.Basil_Counter > 0 &&
		it.Pizzadough_Counter > 0)
	{
		it.Tmato_Counter--;
		it.Basil_Counter--;
		it.Pizzadough_Counter--;
		return PizzaType::Marinara;
	}
	return PizzaType::None;
}

void Player_Managiment::Initialisation()
{
	// 初期位置をタイル->ピクセルに変換して設定
	Player_MovePointX = Player_StanderdpointX * TILE_SIZE;
	Player_MovePointY = Player_StanderdpointY * TILE_SIZE;

	// 画像ロード
	PlayerImage_Handle[0] = LoadGraph("../Pizza_Image/player_up.png");
	PlayerImage_Handle[1] = LoadGraph("../Pizza_Image/player_down.png");
	PlayerImage_Handle[2] = LoadGraph("../Pizza_Image/player_left.png");
	PlayerImage_Handle[3] = LoadGraph("../Pizza_Image/player_right.png");

	Player_Handle = PlayerImage_Handle[3];
	m_dir = Player_EyeContact::PlayerEye_Right;
	m_oldSpace = 0;

	// 時間初期化
	m_lastTime = GetNowCount();
}

void Player_Managiment::Update(const BackScreen& stage, Bllent_Managiment& bllent)
{
	// delta time (秒)
	unsigned int now = GetNowCount();
	float dt = (now - m_lastTime) / 1000.0f;
	if (dt > 0.1f) dt = 0.1f;
	m_lastTime = now;

	// 入力取得（ホールドで滑らか移動）
	int nowUp = CheckHitKey(KEY_INPUT_W);
	int nowDown = CheckHitKey(KEY_INPUT_S);
	int nowLeft = CheckHitKey(KEY_INPUT_A);
	int nowRight = CheckHitKey(KEY_INPUT_D);
	int nowSpace = CheckHitKey(KEY_INPUT_SPACE);



	// 移動ベロシティ(px)
	float vx = 0.0f, vy = 0.0f;
	if (nowLeft) vx -= 1.0f;
	if (nowRight) vx += 1.0f;
	if (nowUp) vy -= 1.0f;
	if (nowDown) vy += 1.0f;

	// 正規化（斜め移動で速くならないように）
	if (vx != 0.0f || vy != 0.0f)
	{
		float len = std::sqrt(vx * vx + vy * vy);
		vx = vx / len * m_moveSpeed;
		vy = vy / len * m_moveSpeed;
	}
	// 予測移動（コライダーは四隅をチェック）
	float newX = Player_MovePointX + vx * dt;
	float newY = Player_MovePointY + vy * dt;

	// プレイヤー矩形（top-left = newX, newY）。表示サイズを当たり判定サイズとして使用
	//int w = m_displaySize;
	//int h = m_displaySize;

	auto collidesAt = [&](float wx, float wy) -> bool {
		return stage.CheckCollision(wx, wy);
	};

	// 四隅チェック（ワールド座標）
	const int HIT_SIZE = 16; // 描画(28px)より小さく
	int w = HIT_SIZE;
	int h = HIT_SIZE;
	float cx = Player_MovePointX + (m_displaySize - HIT_SIZE) / 2.0f;
	float cy = Player_MovePointY + (m_displaySize - HIT_SIZE) / 2.0f;
	bool collision = false;
	if (collidesAt(newX, newY)) collision = true;
	if (collidesAt(newX + w - 1, newY)) collision = true;
	if (collidesAt(newX, newY + h - 1)) collision = true;
	if (collidesAt(newX + w - 1, newY + h - 1)) collision = true;
	
	

	if (!collision)
	{
		// 移動適用
		Player_MovePointX = newX;
		Player_MovePointY = newY;

		// 向き更新（入力があるとき）
		if (vx < 0) { Player_Handle = PlayerImage_Handle[PlayerEye_Left]; m_dir = PlayerEye_Left; }
		else if (vx > 0) { Player_Handle = PlayerImage_Handle[PlayerEye_Right]; m_dir = PlayerEye_Right; }
		else if (vy < 0) { Player_Handle = PlayerImage_Handle[PlayerEye_Up]; m_dir = PlayerEye_Up; }
		else if (vy > 0) { Player_Handle = PlayerImage_Handle[PlayerEye_Down]; m_dir = PlayerEye_Down; }
	}

	// 発射（立ち上がりで1発）
	if (nowSpace == 1 && m_oldSpace == 0)
	{
		// 既存 Shot() はタイル単位を期待しているのでタイルに変換して渡す
		bllent.Shot(static_cast<float>(GetX()), static_cast<float>(GetY()), *this);
	}

	// 前フレーム保存
	m_oldUp = nowUp;
	m_oldDown = nowDown;
	m_oldLeft = nowLeft;
	m_oldRight = nowRight;
	m_oldSpace = nowSpace;
}
