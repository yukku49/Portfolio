#include "DxLib.h"
#include "EnemyManagiment.h"
#include "BackScreenManagiment.h" // For CheckCollision
#include<cmath>

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
	if (a.enemy_X < 0.0f) return a.Enemy_Eye_handlbe[Enemy_Left];
	if (a.enemy_X > 0.0f) return a.Enemy_Eye_handlbe[Enemy_Right];
	if (a.enemy_Y < 0.0f) return a.Enemy_Eye_handlbe[Enemy_Up];
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
void Enemy_Managiment::Enemy_Update(const BackScreen& stage, float playerX, float playerY)
{
	if (!a.isActive) return;

	float dx = playerX - a.enemy_X;
	float dy = playerY - a.enemy_Y;
	float len = std::sqrt(dx * dx + dy * dy);
	if (len < 1.0f) return;

	const float speed = 1.0f;
	float wishVx = (dx / len) * speed;
	float wishVy = (dy / len) * speed;

	const int w = m_displaySize;

	// X軸だけ試す
	float newX = a.enemy_X + wishVx;
	bool hitX =
		stage.CheckCollision(newX, a.enemy_Y) ||
		stage.CheckCollision(newX + w - 1, a.enemy_Y) ||
		stage.CheckCollision(newX, a.enemy_Y + w - 1) ||
		stage.CheckCollision(newX + w - 1, a.enemy_Y + w - 1);

	// Y軸だけ試す
	float newY = a.enemy_Y + wishVy;
	bool hitY =
		stage.CheckCollision(a.enemy_X, newY) ||
		stage.CheckCollision(a.enemy_X + w - 1, newY) ||
		stage.CheckCollision(a.enemy_X, newY + w - 1) ||
		stage.CheckCollision(a.enemy_X + w - 1, newY + w - 1);

	// ぶつからない軸だけ移動を適用
	if (!hitX) a.enemy_X = newX;
	if (!hitY) a.enemy_Y = newY;
};



