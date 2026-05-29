#include "StageState.h"
#include "SpriteRenderer.h"
#include "CameraFollower.h"
#include "Camera.h"
#include "InputManager.h"
#include "TileSet.h"
#include "TileMap.h"
#include "Sarue.h"

StageState::StageState() : State() {
    // 1. O Background Fixo
    GameObject* bgObj = new GameObject();
    bgObj->AddComponent(new SpriteRenderer(*bgObj, "img/Background.png"));
    bgObj->AddComponent(new CameraFollower(*bgObj));
    AddObject(bgObj);

    // 2. O Mapa (Com Tileset.png e map.txt provisórios do trabalho antigo)
    GameObject* mapObj = new GameObject();
    TileSet* tileSet = new TileSet(64, 64, "img/Tileset.png");
    TileMap* tileMap = new TileMap(*mapObj, "map/map.txt", tileSet);
    mapObj->AddComponent(tileMap);
    AddObject(mapObj);

    // 3. O Saruê (Passamos o ponteiro do mapa para ele sentir o chão)
    GameObject* sarueObj = new GameObject();
    sarueObj->box.x = 200; // Posição inicial no mapa
    sarueObj->box.y = 200;
    sarueObj->AddComponent(new Sarue(*sarueObj, tileMap));
    AddObject(sarueObj);

    // A câmera segue o Saruê
    Camera::Follow(sarueObj);
}

StageState::~StageState() {}

void StageState::LoadAssets() {}

void StageState::Update(float dt) {
    InputManager& input = InputManager::GetInstance();
    Camera::Update(dt);

    if (input.KeyPress(SDLK_ESCAPE) || input.QuitRequested()) {
        quitRequested = true;
    }

    UpdateArray(dt);
}

void StageState::Render() {
    RenderArray();
}

void StageState::Start() {
    LoadAssets();
    StartArray();
}

void StageState::Pause() {}
void StageState::Resume() {}