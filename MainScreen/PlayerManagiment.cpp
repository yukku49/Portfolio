#include "PlayerManagiment.h"
#include "ItemManagiment.h"
#include "BackScreenManagiment.h"
#include "BllentManagiment.h"
#include "DatabaseManagiment.h"

#include "DxLib.h"
#include <cmath>

// Tile size in pixels, used for coordinate conversions throughout this file
// (JP: タイルのピクセルサイズ。このファイル全体で座標変換に使う)
static constexpr int TILE_SIZE = 32;

// Increments the inventory counter matching the picked-up ingredient
// Called by the main loop after CheckPickUp() confirms a pickup
// (JP: 拾ったアイテムに対応する所持カウンタを増やす。メインループがCheckPickUp()後に呼ぶ)
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

// Asks the database whether the current inventory satisfies any pizza recipe.
// If a recipe matches:
//   - The database consumes the required ingredients from Player_Itembring
//   - m_fullness increases by the pizza's fullness value
//   - m_pizzaTimers for that pizza type is set (next bullet uses this pizza)
// Returns PizzaType::None when no recipe is satisfied
// (JP: 現在の所持食材でレシピが満たされるかDBに問い合わせる)
// (JP: 成功時: 食材を消費し、満腹ゲージを増やし、ピザタイマーをセットする)
PizzaType Player_Managiment::TryMakePizza()
{
	if (m_db && m_db->IsInitialized())
	{
		const PizzaType type = m_db->TryMakePizza(Player_Itembring, m_pizzaTimers);
		if (type != PizzaType::None)
		{
			//Increase fullness and clamp to maximum
			//(JP:満腹ゲージを増やし、最大値でクランプする）
			m_fullness = m_db->OnPizzaMade(type, m_fullness);
			if (m_fullness > MAX_FULLNES)
			{
				m_fullness = MAX_FULLNES;
			}
			//Increment score each time a pizza is completed
			m_score++;

			//Only set win flag when the fullness bar is completely filled
			//A single pizza is NOT enough to win; the player must fill the bar
			//(JP:満腹ゲージが完全にまぷくになった時だけ勝利フラグを立てる)
			//(JP:ピザ一枚では勝てない。ゲージを満腹にする必要がある)
			if(m_fullness>=MAX_FULLNES)
			m_isWin = true;
		}
		return type;
	}

	OutputDebugStringA("PizzaDatabase not initialized: TryMakePizza failed\n");
	return PizzaType::None;
}

// Debug only: bypasses pickup logic and sets inventory directly
// Useful for testing pizza recipes without walking around the map
// (JP: デバッグ専用。アイテム拾いを経由せず所持数を直接セットする。レシピテスト用)
void Player_Managiment::Debug_SetItems(int dough, int tomato, int cheese, int gorgonzola, int basil)
{
	Player_Itembring.Pizzadough_Counter = dough;
	Player_Itembring.Tmato_Counter = tomato;
	Player_Itembring.Cheese_Counter = cheese;
	Player_Itembring.Gorgonzola_Counter = gorgonzola;
	Player_Itembring.Basil_Counter = basil;

}

void Player_Managiment::PlayerOnHit()
{
	if (mp_invincibleTimer > 0.0f)return;//無敵中は無視
	mp_invincibleTimer = P_INVINCIBLE_DURATION;
	m_fullness -= 5;
	if (m_fullness < 0.0f)m_fullness = 0.0f;
}

// Loads directional sprites and sets the player's starting pixel position
// The starting tile position is multiplied by TILE_SIZE to get pixel coordinates
// An extra TILE_SIZE is added to Y so the player appears below the HUD bar
// (JP: 方向別スプライトを読み込み、プレイヤーの開始ピクセル座標を設定する)
// (JP: タイル座標×TILESIZEでピクセル座標に変換。YにTILESIZEを足してHUD下に配置)
void Player_Managiment::Initialisation()
{
	// 初期位置をタイル->ピクセルに変換して設定
	Player_MovePointX = Player_StanderdpointX * TILE_SIZE;
	Player_MovePointY = Player_StanderdpointY * TILE_SIZE+TILE_SIZE;

	// 画像ロード
	PlayerImage_Handle[0] = LoadGraph("../Pizza_Image/player_up.png");
	PlayerImage_Handle[1] = LoadGraph("../Pizza_Image/player_down.png");
	PlayerImage_Handle[2] = LoadGraph("../Pizza_Image/player_left.png");
	PlayerImage_Handle[3] = LoadGraph("../Pizza_Image/player_right.png");

	// Default facing direction is right
	// (JP: デフォルトの向きは右)
	Player_Handle = PlayerImage_Handle[3];
	m_dir = Player_EyeContact::PlayerEye_Right;
	m_oldSpace = 0;

	// 時間初期化
	m_lastTime = GetNowCount();
}

// Main update: called once per frame from the main loop
// Responsibilities:
//   1. Calculate delta time (dt) for frame-rate-independent movement
//   2. Count down pizza effect timers
//   3. Read WASD input and build a velocity vector
//   4. Normalize velocity so diagonal movement is not faster
//   5. Check four corners of the hit-box against the tile map
//   6. Apply movement only if no collision was detected
//   7. Fire a bullet on Space key press (edge detection, not hold)
// (JP: メインループから毎フレーム呼ばれるメイン更新関数)
void Player_Managiment::Update(const BackScreen& stage, Bllent_Managiment& bllent)
{
	// --- 1. Delta time ---
	// GetNowCount() returns milliseconds; divide by 1000 to get seconds
	// Clamp to 0.1s to avoid huge jumps after frame hitches (e.g. window resize)
	// (JP: GetNowCount()はミリ秒を返す。1000で割って秒に変換。フレーム落ち時の大ジャンプを防ぐため0.1sにクランプ)
	unsigned int now = GetNowCount();
	float dt = (now - m_lastTime) / 1000.0f;
	if (dt > 0.1f)dt = 0.1f;
	m_lastTime = now;

	if (mp_invincibleTimer > 0.0f)
	{
		mp_invincibleTimer -= dt;
		mp_blinkTimer++;
	}
	// --- 2. Count down pizza effect timers ---
	// Each timer starts at a positive value when a pizza is crafted and ticks toward 0
	// BllentManagiment reads these to choose the bullet's pizza type
	// (JP: 各タイマーはピザ完成時に正の値にセットされ、0に向けてカウントダウンする)
	if (m_pizzaTimers.Marigherita > 0.0f) m_pizzaTimers.Marigherita-=dt;
	if (m_pizzaTimers.QuattroFormaggi > 0.0f) m_pizzaTimers.QuattroFormaggi-=dt;
	if (m_pizzaTimers.Genovese > 0.0f) m_pizzaTimers.Genovese-=dt;
	if (m_pizzaTimers.Marinara > 0.0f) m_pizzaTimers.Marinara-=dt;

	

	// --- 3. Read keyboard input ---
	int nowUp = CheckHitKey(KEY_INPUT_W);
	int nowDown = CheckHitKey(KEY_INPUT_S);
	int nowLeft = CheckHitKey(KEY_INPUT_A);
	int nowRight = CheckHitKey(KEY_INPUT_D);
	int nowSpace = CheckHitKey(KEY_INPUT_SPACE);



	// --- 4. Build and normalize velocity vector ---
    // Raw direction vector (components are -1, 0, or +1)
    // (JP: 生の方向ベクトル。各成分は-1, 0, +1のいずれか)
	float vx = 0.0f, vy = 0.0f;
	if (nowLeft) vx -= 1.0f;
	if (nowRight) vx += 1.0f;
	if (nowUp) vy -= 1.0f;
	if (nowDown) vy += 1.0f;

	// Normalize so diagonal movement speed equals single-axis speed
	// Then scale by m_moveSpeed (px/s) and dt (seconds) to get the pixel displacement
	// (JP: 斜め移動が速くならないよう正規化。m_moveSpeed(px/s)×dt(秒)でピクセル変位を求める)
	if (vx != 0.0f || vy != 0.0f)
	{
		float len = std::sqrt(vx * vx + vy * vy);
		vx = vx / len * m_moveSpeed;
		vy = vy / len * m_moveSpeed;
	}

// Candidate new position after this frame's movement
    // (JP: このフレームの移動後の候補新座標)
	float newX = Player_MovePointX + vx * dt;
	float newY = Player_MovePointY + vy * dt;

	// プレイヤー矩形（top-left = newX, newY）。表示サイズを当たり判定サイズとして使用
	//int w = m_displaySize;
	//int h = m_displaySize;

	auto collidesAt = [&](float wx, float wy) -> bool {
		return stage.CheckCollision(wx, wy);
	};

	// --- 5. Four-corner collision check ---
 // Use a hit-box smaller than the drawn sprite so the player can pass through
 // narrow gaps that look visually clear
 // The hit-box is centered horizontally within the sprite
 // (JP: 描画スプライトより小さいヒットボックスを使う。見た目的に空いている隙間を通れるようにする)
 // (JP: ヒットボックスはスプライト内で水平方向に中央揃え)
	const int HIT_SIZE = 16; 
	int w = HIT_SIZE;
	int h = HIT_SIZE;
	float hitX = newX + (m_displaySize - HIT_SIZE) / 2.0f;
	float hitY = newY;
	bool collision = false;
	if (collidesAt(hitX, hitY)) collision = true;
	if (collidesAt(hitX + w - 1, hitY)) collision = true;
	if (collidesAt(hitX, hitY + h - 1)) collision = true;
	if (collidesAt(hitX + w - 1, hitY + h - 1)) collision = true;

	
	
	// --- 6. Apply movement and update facing direction ---
	// Only move if none of the four corners hit an obstacle
	// (JP: 4隅のどれも障害物に当たっていない場合のみ移動を適用する)
	if (!collision)
	{
		// 移動適用
		Player_MovePointX = newX;
		Player_MovePointY = newY;

		// Update the sprite handle to match the dominant movement axis
	   // (JP: 主な移動軸に合わせてスプライトハンドルを更新する)
		if (vx < 0) { Player_Handle = PlayerImage_Handle[PlayerEye_Left]; m_dir = PlayerEye_Left; }
		else if (vx > 0) { Player_Handle = PlayerImage_Handle[PlayerEye_Right]; m_dir = PlayerEye_Right; }
		else if (vy < 0) { Player_Handle = PlayerImage_Handle[PlayerEye_Up]; m_dir = PlayerEye_Up; }
		else if (vy > 0) { Player_Handle = PlayerImage_Handle[PlayerEye_Down]; m_dir = PlayerEye_Down; }
	}

	// --- 7. Shoot on Space key press (edge detection) ---
	// Fire only on the frame the key transitions from released to pressed
	// This prevents holding Space from spamming bullets
	// (JP: スペースキーを押した瞬間のフレームのみ発射する)
	// (JP: ホールドで連射されないよう、前フレームが離れていて今フレームが押されている場合のみ発射)
	if (nowSpace == 1 && m_oldSpace == 0)
	{
		bllent.Shot(*this);
	}

	// Save this frame's key states for next frame's edge detection
	// (JP: 次フレームのエッジ検出のために今フレームのキー状態を保存する)
	m_oldUp = nowUp;
	m_oldDown = nowDown;
	m_oldLeft = nowLeft;
	m_oldRight = nowRight;
	m_oldSpace = nowSpace;
}
// デバッグ用：素材を直接セット

