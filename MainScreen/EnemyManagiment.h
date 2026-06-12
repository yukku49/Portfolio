#pragma once
#include<vector>
#include<utility>
// Enum for enemy facing sprite indices (JP: 敵の向きスプライトのインデックス)
enum enemy_Eye
{
	Enemy_Up,
	Enemy_Left,
	Enemy_Right,
	Enemy_Down,
	Enemy_Max
};

struct Enemy_date
{
	float enemy_X = 0.0f; // 敵の X（ピクセル単位）
	float enemy_Y = 0.0f; // 敵の Y（ピクセル単位）
	float vx = 0.0f;      // X velocity (px per update)
	float vy = 0.0f;      // Y velocity
	int fullness_gauge = 0;
	bool isActive = false;
	int Enemy_Eye_handlbe[enemy_Eye::Enemy_Max];
};

class BackScreen; // Forward declaration for collision check
class PlayerManagiment;

class Enemy_Managiment
{
private:
	Enemy_date a;
	// プレイヤーと同じ描画サイズ（px）
	const int m_displaySize = 28;

	//BFS関連
	std::vector<std::pair<int, int>> m_path;
	int m_pathIndex = 0;
	int m_pathTimer = 0;
	static const int PATH_INTERVAL = 60;

	void CalcPath(const BackScreen& stage,
		int startTx, int startTy,
		int goalTx, int goalTy);
public:
	// Spawn and initialize enemy (startX/startY are pixel coordinates)
	void Enemy_Initialisation(float startX, float startY);

	// Getter for enemy state
	const Enemy_date& Get_enemyPoint() const { return a; }

	// Update enemy movement (既存：フレーム単位で単純移動)
	void Enemy_Update();

	// 新規：当たり判定ありの更新（BackScreen を渡す）
	void Enemy_Update(const BackScreen& stage,float playerX,float playerY);


	// DrawManager 用 getter

	float Get_enemyX() const { return a.enemy_X; }

	float Get_enemyY() const { return a.enemy_Y; }

	int Get_EnemyHandle() const;

	int Get_EnemyDisplaySize() const { return m_displaySize; }

	// 被弾処理：単純に無効化（HP 等あれば拡張する）
	void OnHit() { a.isActive = false; }
	// 有効判定（Bllent から参照されるため）
	bool Get_EnemyActive() const { return a.isActive; }
	
	int GetPathSize() const { return (int)m_path.size(); }
	std::pair<int, int> GetPath(int i) const { return m_path[i]; }
};