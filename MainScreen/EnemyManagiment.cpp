#include "DxLib.h"
#include "EnemyManagiment.h"
#include "BackScreenManagiment.h" // For CheckCollision
#include<cmath>
#include<queue>
#include<array>
#include<algorithm>

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

<<<<<<< HEAD
=======
    // ★ オフセットなしでタイル座標に変換
>>>>>>> bf210bda756864ed957e0258e0c5621390aa6722
    int eTx = static_cast<int>(a.enemy_X) / 32;
    int eTy = static_cast<int>(a.enemy_Y) / 32;
    int pTx = static_cast<int>(playerX) / 32;
    int pTy = static_cast<int>(playerY) / 32;

    m_pathTimer++;
    if (m_pathTimer >= PATH_INTERVAL || m_path.empty())
    {
        m_pathTimer = 0;
        CalcPath(stage, eTx, eTy, pTx, pTy);
<<<<<<< HEAD
        m_pathIndex = 0; // ← インデックスリセット忘れずに
=======
        m_pathIndex = 0;
>>>>>>> bf210bda756864ed957e0258e0c5621390aa6722
    }

    if (m_path.empty() || m_pathIndex >= (int)m_path.size()) return;

<<<<<<< HEAD
    // 次のタイル
    auto [nextTx, nextTy] = m_path[m_pathIndex];
    float nextPx = nextTx * 32.0f;
    float nextPy = nextTy * 32.0f;

    float dx = nextPx - a.enemy_X;
    float dy = nextPy - a.enemy_Y;
    float dist = std::sqrt(dx * dx + dy * dy);

    // 到達したら次のタイルへ
=======
    auto [nextTx, nextTy] = m_path[m_pathIndex];

    // ★ オフセットなしでピクセルに変換
    float nextPx = nextTx * 32.0f;
    float nextPy = nextTy * 32.0f;

    float dx = nextPx - a.enemy_X;
    float dy = nextPy - a.enemy_Y;
    float dist = std::sqrt(dx * dx + dy * dy);

>>>>>>> bf210bda756864ed957e0258e0c5621390aa6722
    if (dist < 2.0f)
    {
        a.enemy_X = nextPx;
        a.enemy_Y = nextPy;
        m_pathIndex++;
        return;
    }

    const float speed = 1.5f;
    float moveX = (dx / dist) * speed;
    float moveY = (dy / dist) * speed;

    const int w = m_displaySize;

<<<<<<< HEAD
    // ★ X軸衝突判定
    float newX = a.enemy_X + moveX;
<<<<<<< HEAD
   
=======
>>>>>>> parent of bf210bd (コメントなし)
    bool hitX =
        stage.CheckCollision(newX, a.enemy_Y) ||
        stage.CheckCollision(newX + w - 1, a.enemy_Y) ||
        stage.CheckCollision(newX, a.enemy_Y + w - 1) ||
<<<<<<< HEAD
        stage.CheckCollision(newX + w - 1, a.enemy_Y+ w - 1);
=======
        stage.CheckCollision(newX + w - 1, a.enemy_Y + w - 1);
>>>>>>> parent of bf210bd (コメントなし)

    // ★ Y軸衝突判定
    float newY = a.enemy_Y + moveY;
    bool hitY =
        stage.CheckCollision(a.enemy_X, newY) ||
        stage.CheckCollision(a.enemy_X + w - 1, newY) ||
        stage.CheckCollision(a.enemy_X, newY + w - 1) ||
        stage.CheckCollision(a.enemy_X + w - 1, newY + w - 1);
=======
    float newX = a.enemy_X + moveX;
    float hitBaseY = a.enemy_Y + 32.0f;
    bool hitX =
        stage.CheckCollision(newX, hitBaseY) ||
        stage.CheckCollision(newX + w - 1, hitBaseY) ||
        stage.CheckCollision(newX, hitBaseY + w - 1) ||
        stage.CheckCollision(newX + w - 1, hitBaseY + w - 1);

    float newY = a.enemy_Y + moveY;
    bool hitY =
        stage.CheckCollision(a.enemy_X, newY+32.0f) ||
        stage.CheckCollision(a.enemy_X + w - 1, newY+32.0f) ||
        stage.CheckCollision(a.enemy_X, newY+32.0f + w - 1) ||
        stage.CheckCollision(a.enemy_X + w - 1, newY+32.0f + w - 1);
>>>>>>> bf210bda756864ed957e0258e0c5621390aa6722

    if (!hitX) a.enemy_X = newX;
    if (!hitY) a.enemy_Y = newY;

<<<<<<< HEAD
    // ★ 衝突が続くなら再計算を強制
    if (hitX && hitY)
    {
        m_pathTimer = PATH_INTERVAL;
    }
=======
    if (hitX && hitY) m_pathTimer = PATH_INTERVAL;
>>>>>>> bf210bda756864ed957e0258e0c5621390aa6722
};

void Enemy_Managiment::CalcPath(const BackScreen& stage,
    int startTx, int startTy, int goalTx, int goalTy)
{
    m_path.clear();
    m_pathIndex = 0;

    const int W = stage.MAP_Get_SizeX();
    const int H = stage.MAP_Get_SizeY();

    // 範囲外チェック
    if (goalTx < 0 || goalTx >= W || goalTy < 0 || goalTy >= H) return;
    if (stage.GetMapvalue(goalTx, goalTy) == 0) return;

    // 親座標を記録する2次元配列
    std::vector<std::vector<std::pair<int, int>>> parent(
        H, std::vector<std::pair<int, int>>(W, { -1, -1 }));

    std::queue<std::pair<int, int>> q;
    q.push({ startTx, startTy });
    parent[startTy][startTx] = { startTx, startTy };

    const std::array<std::pair<int, int>, 4> dirs = { {{0,-1},{0,1},{-1,0},{1,0}} };

    bool found = false;
    while (!q.empty() && !found)
    {
        auto [cx, cy] = q.front(); q.pop();

        for (auto [dx, dy] : dirs)
        {
            int nx = cx + dx;
            int ny = cy + dy;

            if (nx < 0 || nx >= W || ny < 0 || ny >= H) continue;
            if (stage.GetMapvalue(nx, ny) == 0) continue;
            if (parent[ny][nx].first != -1) continue;

            parent[ny][nx] = { cx, cy };
            q.push({ nx, ny });

            if (nx == goalTx && ny == goalTy)
            {
                found = true;
                break;
            }
        }
    }

    if (!found) return;

    // ゴールから逆順にたどる
    std::pair<int, int> cur = { goalTx, goalTy };
    while (cur != std::make_pair(startTx, startTy))
    {
        m_path.push_back(cur);
        cur = parent[cur.second][cur.first];
    }

    std::reverse(m_path.begin(), m_path.end());
}


