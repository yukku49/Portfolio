#pragma once

// Use forward declarations to avoid circular includes (JP: 循環インクルードを避けるため前方宣言)
class BackScreen;
class Player_Managiment;
class Bllent_Managiment;
class Enemy_Managiment;
class Item_Managiment;

class DrawManager
{
public:
    void Player_Draw(const BackScreen& stage, const Player_Managiment& player) const;
    void Map_Draw(const BackScreen& object) const;
    void Bullets_Draw(const Bllent_Managiment& bullets) const;
    void Enemy_Draw(const Enemy_Managiment& enemy, const BackScreen& stage) const;
    void Item_Draw(const Item_Managiment& item, const BackScreen& stage)const;
};