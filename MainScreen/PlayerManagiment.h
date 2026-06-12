#pragma once
#include"ItemTypes.h"
// Avoid including DrawManagiment.h here (JP: ここではDrawManagiment.hをインクルードしない)
#include <array>

enum Player_EyeContact
{
	PlayerEye_Up = 0,
	PlayerEye_Down,
	PlayerEye_Left,
	PlayerEye_Right,
	PlayerEye_Max
};

class Item_Managiment;
class Bllent_Managiment;
class BackScreen; // Forward declaration (JP: 前方宣言)
class PizzaDatabase;

class Player_Managiment
{
private:
	// タイル基準の初期値（マス単位）
	const float Player_StanderdpointX = 1.0f;
	const float Player_StanderdpointY = 1.0f;

	// 内部座標はピクセル単位で保持する（float）
	float Player_MovePointX = 0.0f; // px
	float Player_MovePointY = 0.0f; // px

	// Player image handles by direction (JP: 向き別プレイヤー画像ハンドル)
	int PlayerImage_Handle[Player_EyeContact::PlayerEye_Max];

	// Current player draw handle (JP: 現在の描画用ハンドル)
	int Player_Handle = 0;

	const int m_displaySize = 28; // 描画サイズ(px/概ね)

	// 前回キー状態（立ち上がり検出用）
	int m_oldUp = 0, m_oldDown = 0, m_oldLeft = 0, m_oldRight = 0;
	int m_oldSpace = 0;

	// 向き
	int m_dir;

	// 所持材料構造体
	Item_count Player_Itembring;

	// 移動パラメータ（ピクセル単位）
	float m_moveSpeed = 128.0f; // px/s（調整可）

	// 時間管理
	unsigned int m_lastTime = 0;

	//満腹
	float m_fullness = 0.0f;
	


	// ピザ発射のクールダウン管理
	PizzaTimer m_pizzaTimers;

	PizzaDatabase* m_db = nullptr;

public:
	static constexpr float MAX_FULLNES = 100.0f;

	void Initialisation();
	void SetDatabase(PizzaDatabase* db) { m_db = db; }
	void Update(const BackScreen& stage, Bllent_Managiment& bllent);

	// 互換性維持：既存コード向けにタイル単位 getter（戻り値はタイル index）
	int GetX() const { return static_cast<int>(Player_MovePointX) / 32; }
	int GetY() const { return static_cast<int>(Player_MovePointY) / 32; }

	// 新：ピクセル単位の float getter（描画や弾の発射などに使う）
	float GetXf() const { return Player_MovePointX; } // px
	float GetYf() const { return Player_MovePointY; } // px

	// スプライトハンドル getter（綴り差異に備え2つ用意）
	int Get_PlayerHanadle() const { return Player_Handle; }
	int Get_PlayerHandle() const { return Player_Handle; }

	// 所持アイテム取得
	Item_count Get_Player_Itembring() const { return Player_Itembring; };

	// カウンタを増やす（JP: 所持アイテムのカウンタを増やす）
	void Player_BringItem(Item_number picked);

	// 向きの取得（JP: Getter for current facing direction）
	int GetDir() const { return m_dir; };

	// 描画用サイズ（JP: Getter for display size)
	int Get_PlayerDisplaySize() const { return m_displaySize; }
	// ピザを作れるか試す（JP: Check if a pizza can be made and return its type）
	PizzaType TryMakePizza();

	//
	float GetFullness()const { return m_fullness; };
	
	const PizzaTimer& GetPizzaTimer()const { return m_pizzaTimers; };

	// デバッグ用：アイテムカウンタを直接設定（JP: Debug function to set item counters directly）
	//void Debug_SetItems(int dough, int tomato, int cheese, int gorgonzola, int basil);


};