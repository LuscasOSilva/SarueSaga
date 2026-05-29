#include "Sarue.h"
#include "InputManager.h"
#include "Sprite.h"
#include "Animator.h"
#include "Camera.h"

Sarue* Sarue::player = nullptr;

Sarue::Sarue(GameObject& associated, TileMap* map) : Component(associated) {
    player = this;
    this->map = map;
    speed = {0, 0};
    isGrounded = false;
    isFacingLeft = false;

    // Carrega a imagem do Saruê (O Player.png do trabalho antigo)
    Sprite* sprite = new Sprite(associated, "img/Player.png");
    associated.AddComponent(sprite);

    // Configura o Animador com os frames pedidos
    Animator* anim = new Animator(associated);
    
    // Animações básicas (ajuste os frames se ele tiver animação de andar na sua spritesheet)
    anim->AddAnimation("idle", Animation(0, 0, 1.0f)); 
    anim->AddAnimation("run", Animation(1, 4, 0.1f));  
    
    // A mágica do pulo: Frame 5 para subir, Frame 6 para cair
    anim->AddAnimation("jumpUp", Animation(5, 5, 1.0f)); 
    anim->AddAnimation("fallDown", Animation(6, 6, 1.0f));
    
    associated.AddComponent(anim);
}

Sarue::~Sarue() {
    if (player == this) {
        player = nullptr;
    }
}

void Sarue::Update(float dt) {
    InputManager& input = InputManager::GetInstance();

    // 1. --- CONTROLE DO JOGADOR ---
    speed.x = 0;
    if (input.IsKeyDown(SDLK_a) || input.IsKeyDown(SDLK_LEFT)) {
        speed.x = -MOVE_SPEED;
        isFacingLeft = true;
    }
    if (input.IsKeyDown(SDLK_d) || input.IsKeyDown(SDLK_RIGHT)) {
        speed.x = MOVE_SPEED;
        isFacingLeft = false;
    }

    if ((input.KeyPress(SDLK_SPACE) || input.KeyPress(SDLK_UP)) && isGrounded) {
        speed.y = -JUMP_FORCE; // Impulso para cima
        isGrounded = false;
    }

    // 2. --- FÍSICA E GRAVIDADE ---
    speed.y += GRAVITY * dt;
    if (speed.y > MAX_FALL_SPEED) speed.y = MAX_FALL_SPEED;

    float dtSpeedX = speed.x * dt;
    float dtSpeedY = speed.y * dt;

    // 3. --- COLISÃO EIXO X ---
    float nextX = associated.box.x + dtSpeedX;
    bool colidiuX = false;
    
    // Diminuímos 1 pixel da altura para ele não tropeçar nas quinas do chão
    float margin = 1.0f; 

    if (speed.x > 0) { // Indo para Direita
        if (map->IsSolid(nextX + associated.box.w, associated.box.y + margin) || 
            map->IsSolid(nextX + associated.box.w, associated.box.y + associated.box.h - margin)) {
            speed.x = 0;
            associated.box.x = ((int)(nextX + associated.box.w) / map->GetTileWidth()) * map->GetTileWidth() - associated.box.w - 0.1f;
            colidiuX = true;
        }
    } else if (speed.x < 0) { // Indo para Esquerda
        if (map->IsSolid(nextX, associated.box.y + margin) || 
            map->IsSolid(nextX, associated.box.y + associated.box.h - margin)) {
            speed.x = 0;
            associated.box.x = ((int)(nextX / map->GetTileWidth()) + 1) * map->GetTileWidth() + 0.1f;
            colidiuX = true;
        }
    }
    if (!colidiuX) associated.box.x = nextX;


    // 4. --- COLISÃO EIXO Y ---
    float nextY = associated.box.y + dtSpeedY;
    bool colidiuY = false;
    isGrounded = false;

    if (speed.y > 0) { // Caindo
        if (map->IsSolid(associated.box.x + margin, nextY + associated.box.h) || 
            map->IsSolid(associated.box.x + associated.box.w - margin, nextY + associated.box.h)) {
            speed.y = 0;
            associated.box.y = ((int)(nextY + associated.box.h) / map->GetTileHeight()) * map->GetTileHeight() - associated.box.h - 0.1f;
            isGrounded = true;
            colidiuY = true;
        }
    } else if (speed.y < 0) { // Subindo (Pulo)
        if (map->IsSolid(associated.box.x + margin, nextY) || 
            map->IsSolid(associated.box.x + associated.box.w - margin, nextY)) {
            speed.y = 0; // Bate a cabeça no teto e começa a cair
            associated.box.y = ((int)(nextY / map->GetTileHeight()) + 1) * map->GetTileHeight() + 0.1f;
            colidiuY = true;
        }
    }
    if (!colidiuY) associated.box.y = nextY;


    // 5. --- LIMITES DA TELA (Como a câmera ainda não mexe) ---
    if (associated.box.x < 0) associated.box.x = 0;
    if (associated.box.x + associated.box.w > 1024) associated.box.x = 1024 - associated.box.w;
    
    if (associated.box.y < 0) {
        associated.box.y = 0;
        speed.y = 0;
    }
    if (associated.box.y + associated.box.h >= 600) { // Chão seguro de teste
        associated.box.y = 600 - associated.box.h;
        speed.y = 0;
        isGrounded = true;
    }

    // 6. --- CONTROLE DE ANIMAÇÕES E DIREÇÃO ---
    Animator* anim = (Animator*)associated.GetComponent("Animator");
    if (anim != nullptr) {
        if (!isGrounded) {
            if (speed.y < 0) {
                anim->SetAnimation("jumpUp"); // Frame 5
            } else {
                anim->SetAnimation("fallDown"); // Frame 6
            }
        } else {
            if (speed.x != 0) {
                anim->SetAnimation("run");
            } else {
                anim->SetAnimation("idle");
            }
        }
    }
}

void Sarue::Render() {}
bool Sarue::Is(std::string type) { return type == "Sarue"; }