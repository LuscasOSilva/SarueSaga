#include "StageState.h"
#include "SpriteRenderer.h"
#include "CameraFollower.h"
#include "Camera.h"
#include "InputManager.h"
#include "Sarue.h"

StageState::StageState() : State() {
    // 1. O Background Fixo
    GameObject* bgObj = new GameObject();
    bgObj->AddComponent(new SpriteRenderer(*bgObj, "img/Background.png"));
    bgObj->AddComponent(new CameraFollower(*bgObj));
    AddObject(bgObj);

    // 2. O Saruê (Sem mapa por enquanto, testaremos a física pura)
    GameObject* sarueObj = new GameObject();
    sarueObj->box.x = 200; 
    sarueObj->box.y = 100; // Nasce caindo do céu
    sarueObj->AddComponent(new Sarue(*sarueObj, nullptr)); // <-- map nulo
    AddObject(sarueObj);

    // Camera::Follow(sarueObj);
}

StageState::~StageState() {}
void StageState::LoadAssets() {}

void StageState::Update(float dt) {
    InputManager& input = InputManager::GetInstance();
    Camera::Update(dt);

    if (input.KeyPress(SDLK_ESCAPE) || input.QuitRequested()) quitRequested = true;

    UpdateArray(dt);
}

void StageState::Render() { RenderArray(); }
void StageState::Start() { LoadAssets(); StartArray(); }
void StageState::Pause() {}
void StageState::Resume() {}