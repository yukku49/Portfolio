#include "DxLib.h"
#include "EnemyManagiment.h"
#include "BackScreenManagiment.h" // For CheckCollision

// 画面サイズやスプライトサイズが必要なら共通化してください
static const int SCREEN_W = 1280;
static const int SCREEN_H = 736;
static const float DEFAULT_SPEED = -2.0f;

void Enemy_Managiment::Enemy_Initialisation(float startX, float startY)
{
	// startX/startY はピクセル単位で受け取る想定
	a.enemy_X = startX;
	a.enemy_Y = startY;
	a.vx = DEFAULT_SPEED; // px per update
	a.vy = 0.0f;
	a.isActive = true;

	// 画像は初期化時一度だけロードする
	a.Enemy_Eye_handlbe[Enemy_Up] = LoadGraph("../Pizza_Image/Enemy_Up.png");
	a.Enemy_Eye_handlbe[Enemy_Left] = LoadGraph("../Pizza_Image/Enemy_Left.png");
	a.Enemy_Eye_handlbe[Enemy_Right] = LoadGraph("../Pizza_Image/Enemy_Right.png");
	a.Enemy_Eye_handlbe[Enemy_Down] = LoadGraph("../Pizza_Image/Enemy_Down.png");
}

int Enemy_Managiment::Get_EnemyHandle() const
{
	// 簡易：vx/vy によって向きを推定してハンドルを返す
	if (!a.isActive) return -1;
	if (a.vx < 0.0f) return a.Enemy_Eye_handlbe[Enemy_Left];
	if (a.vx > 0.0f) return a.Enemy_Eye_handlbe[Enemy_Right];
	if (a.vy < 0.0f) return a.Enemy_Eye_handlbe[Enemy_Up];
	return a.Enemy_Eye_handlbe[Enemy_Down];
}

// 既存の簡易更新（互換性維持）——当たり判定無し
void Enemy_Managiment::Enemy_Update()
{
	if (!a.isActive) return;

	// 単純な移動（既存挙動）
	a.enemy_X += a.vx;
	a.enemy_Y += a.vy;

	// 端で反転（スクリーン端を想定）
	if (a.enemy_X < 0.0f || a.enemy_X > SCREEN_W - 32.0f)
	{
		a.vx *= -1.0f;
	}
	if (a.enemy_Y < 0.0f || a.enemy_Y > SCREEN_H - 32.0f)
	{
		a.vy *= -1.0f;
	}
}

// 新規：BackScreen を用いた当たり判定あり更新（四隅サンプル）
void Enemy_Managiment::Enemy_Update(const BackScreen& stage)
{
	if (!a.isActive) return;

	// 予測移動（vx/vy はピクセル単位でフレーム毎）
	float newX = a.enemy_X + a.vx;
	float newY = a.enemy_Y + a.vy;

	int w = m_displaySize;
	int h = m_displaySize;

	auto collidesAt = [&](float wx, float wy) -> bool {
		return stage.CheckCollision(wx, wy); // world(px) -> tile に変換して判定
	};

	// 四隅チェック（ワールド座標）
	bool collision = false;
	if (collidesAt(newX, newY)) collision = true;
	if (collidesAt(newX + w - 1, newY)) collision = true;
	if (collidesAt(newX, newY + h - 1)) collision = true;
	if (collidesAt(newX + w - 1, newY + h - 1)) collision = true;

	if (!collision)
	{
		// 衝突がなければ移動適用
		a.enemy_X = newX;
		a.enemy_Y = newY;
	}
	else
	{
		// 衝突がある場合は速度を反転（簡易処理）
		a.vx *= -1.0f;
		a.vy *= -1.0f;
	}

	// 画面端反転も適用（安全策）
	if (a.enemy_X < 0.0f) { a.enemy_X = 0.0f; a.vx *= -1.0f; }
	if (a.enemy_X > SCREEN_W - w) { a.enemy_X = SCREEN_W - w; a.vx *= -1.0f; }
	if (a.enemy_Y < 0.0f) { a.enemy_Y = 0.0f; a.vy *= -1.0f; }
	if (a.enemy_Y > SCREEN_H - h) { a.enemy_Y = SCREEN_H - h; a.vy *= -1.0f; }
}


