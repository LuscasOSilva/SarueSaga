#ifndef SARUE_H
#define SARUE_H

#include "Component.h"
#include "Vec2.h"
#include "TileMap.h"

class Sarue : public Component {
public:
    Sarue(GameObject& associated, TileMap* map);
    ~Sarue();

    void Update(float dt) override;
    void Render() override;
    bool Is(std::string type) override;

    static Sarue* player; // Ponteiro global igual tínhamos antes

private:
    TileMap* map;
    Vec2 speed;
    
    bool isGrounded;
    bool isFacingLeft;

    // Constantes de Física (Ajuste para dar o peso do pulo estilo Hollow Knight)
    const float GRAVITY = 1500.0f;     
    const float JUMP_FORCE = 600.0f;   
    const float MOVE_SPEED = 300.0f;   
    const float MAX_FALL_SPEED = 800.0f; 
};

#endif