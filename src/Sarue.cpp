#include "Sarue.h"
#include "InputManager.h"
#include "SpriteRenderer.h"
#include "Animator.h"
#include "Camera.h"
#include "SDL2/SDL.h"

Sarue* Sarue::player = nullptr;

Sarue::Sarue(GameObject& associated, TileMap* map) : Component(associated) {
    player = this;
    this->map = map;
    speed = {0, 0};
    isGrounded = false;
    isFacingLeft = false;

    // Carrega a imagem do Saruê (O Player.png do trabalho antigo)
    // Carrega a imagem do Saruê, avisando que a folha tem 7 frames e a base de tempo é 0.1f
    SpriteRenderer* sprite = new SpriteRenderer(associated, "img/Player.png", 3, 4);
    associated.AddComponent(sprite);

    // Configura o Animador com os frames pedidos
    Animator* anim = new Animator(associated);
    
    // Animações básicas (ajuste os frames se ele tiver animação de andar na sua spritesheet)
    anim->AddAnimation("idle", Animation(6, 9, 1.0f)); 
    anim->AddAnimation("walking", Animation(0, 5, 0.1f));  
    
    // A mágica do pulo: Frame 5 para subir, Frame 6 para cair
    anim->AddAnimation("jumpUp", Animation(4, 4, 1.0f)); 
    anim->AddAnimation("fallDown", Animation(5, 5, 1.0f));
    
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
    float margin = 1.0f; 

    if (map != nullptr) {
        if (speed.x > 0) {
            if (map->IsSolid(nextX + associated.box.w, associated.box.y + margin) || 
                map->IsSolid(nextX + associated.box.w, associated.box.y + associated.box.h - margin)) {
                speed.x = 0;
                associated.box.x = ((int)(nextX + associated.box.w) / map->GetTileWidth()) * map->GetTileWidth() - associated.box.w - 0.1f;
                colidiuX = true;
            }
        } else if (speed.x < 0) {
            if (map->IsSolid(nextX, associated.box.y + margin) || 
                map->IsSolid(nextX, associated.box.y + associated.box.h - margin)) {
                speed.x = 0;
                associated.box.x = ((int)(nextX / map->GetTileWidth()) + 1) * map->GetTileWidth() + 0.1f;
                colidiuX = true;
            }
        }
    }
    if (!colidiuX) associated.box.x = nextX;


    // 5. --- COLISÃO EIXO Y (Chão e Teto) ---
    float nextY = associated.box.y + dtSpeedY;
    bool colidiuY = false;
    isGrounded = false;

    if (map != nullptr) {
        if (speed.y > 0) {
            if (map->IsSolid(associated.box.x + margin, nextY + associated.box.h) || 
                map->IsSolid(associated.box.x + associated.box.w - margin, nextY + associated.box.h)) {
                speed.y = 0;
                associated.box.y = ((int)(nextY + associated.box.h) / map->GetTileHeight()) * map->GetTileHeight() - associated.box.h - 0.1f;
                isGrounded = true;
                colidiuY = true;
            }
        } else if (speed.y < 0) {
            if (map->IsSolid(associated.box.x + margin, nextY) || 
                map->IsSolid(associated.box.x + associated.box.w - margin, nextY)) {
                speed.y = 0; 
                associated.box.y = ((int)(nextY / map->GetTileHeight()) + 1) * map->GetTileHeight() + 0.1f;
                colidiuY = true;
            }
        }
    }
    if (!colidiuY) associated.box.y = nextY;


    // 6. --- LIMITES DA TELA E O CHÃO VERDE ---
    // Impede o Saruê de sair pelas laterais da janela
    if (associated.box.x < 0) associated.box.x = 0;
    if (associated.box.x + associated.box.w > 1200) associated.box.x = 1200 - associated.box.w;
    
    // Teto
    if (associated.box.y < 0) {
        associated.box.y = 0;
        speed.y = 0;
    }

    // O PISO MATEMÁTICO (A linha onde a grama começa no Background.png)
    float groundLevel = 515.0f; // AJUSTE DO CHAO
    
    // Se a base do Saruê (Y + Altura) passar da linha da grama:
    if (associated.box.y + associated.box.h >= groundLevel) { 
        associated.box.y = groundLevel - associated.box.h; // Trava o pé dele na grama
        speed.y = 0;
        isGrounded = true; // Avisa o jogo que pode pular de novo!
    }

    // 6. --- CONTROLE DE ANIMAÇÕES E DIREÇÃO --
    // Vira o Sprite para a esquerda ou direita
    SpriteRenderer* sprite = (SpriteRenderer*)associated.GetComponent("SpriteRenderer");
    if (sprite != nullptr) {
        // Se isFacingLeft for verdadeiro, passa FLIP_HORIZONTAL, senão passa FLIP_NONE
        sprite->SetFlip(isFacingLeft ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
    }

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
                anim->SetAnimation("walking");
            } else {
                anim->SetAnimation("idle");
            }
        }
    }
}

void Sarue::Render() {}
bool Sarue::Is(std::string type) { return type == "Sarue"; }