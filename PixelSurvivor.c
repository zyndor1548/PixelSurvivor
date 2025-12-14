#include "raylib-lib/src/raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raylib-lib/src/raygui.h"
#include <stdlib.h>
#include <time.h>

typedef enum GameState {
    MENU = 0,
    PLAYING,
    PAUSED,
    GAMEOVER
} GameState;

#define GAME_NAME "PixelSurvivor"
#define WIDTH	800
#define HEIGHT	450
#define MAX_FPS 60
#define MAX_LEFT -100
#define MAX_RIGHT 615
#define CENTER	257
#define FOE_START -120
#define MAX_FOE_BOTTOM 350
void Core();
void DrawMenuScreen();
void DrawGameOverScreen();
void ResetGame();
int SetX(int val);
void UpdateFoe();
void UpdateBullet();
void Shoot();
int RangeRandom(int min, int max) {
    return min + rand() % (max - min + 1);
}

Texture2D ped,Foe,BulletTxd,Blast,Background; 
Vector2 PlayerPos = {CENTER, 330};
Vector2 FoePos[5] = {{CENTER,FOE_START},{CENTER,FOE_START},{CENTER,FOE_START},{CENTER,FOE_START},{CENTER,FOE_START}};
Vector2	BulletPos[10],BlastPos[5];
Sound GunShot,ReloadSound;
Music bgMusic;
bool BulletActive[10] = {false};
float reloadTimer = 0.0f;
float reloadDelay = 1.0f;
bool isReloading = false;
bool Blasting[5] = {false};
float BlastTimer[5] = {0.0f};
GameState gameState = MENU;
int Level = 1,
	Kills = 0,
	Bullets = 5,
	Life = 5,
	FoeCount = 1,
	BulletCount = 0,
	MaxBullets = 5;

int main(void){
    InitWindow(WIDTH, HEIGHT, GAME_NAME);
    SetExitKey(KEY_NULL);
    SetTargetFPS(MAX_FPS);
	srand(time(NULL));
    Image img = LoadImage("resources/images/girl.png");
    ped = LoadTextureFromImage(img);
	UnloadImage(img);
	img = LoadImage("resources/images/bullet.png");
    BulletTxd = LoadTextureFromImage(img);
	UnloadImage(img);
	img = LoadImage("resources/images/blast.png");
    Blast = LoadTextureFromImage(img);
	UnloadImage(img);
	img = LoadImage("resources/images/foe.png");
	Foe = LoadTextureFromImage(img);
	UnloadImage(img);
	img = LoadImage("resources/images/background.png");
	Background = LoadTextureFromImage(img);
	UnloadImage(img);
	InitAudioDevice();
	Wave wave = LoadWave("resources/audio/gun_shot.wav");
	GunShot = LoadSoundFromWave(wave);
	UnloadWave(wave);
	wave = LoadWave("resources/audio/reload.wav");
	ReloadSound = LoadSoundFromWave(wave);
	UnloadWave(wave);
	bgMusic = LoadMusicStream("resources/audio/background.mp3");
	PlayMusicStream(bgMusic);
	FoePos[0].x = RangeRandom(MAX_LEFT,MAX_RIGHT);
    while (!WindowShouldClose())
    {
		UpdateMusicStream(bgMusic);
		
		if(!IsWindowFocused()&& gameState == PLAYING) gameState = PAUSED; 
		switch(gameState) {
			case MENU:
			case PAUSED:
				DrawMenuScreen();
				break;
			case PLAYING:
				Core();
				break;
			case GAMEOVER:
				DrawGameOverScreen();
				break;
		}
    }
	UnloadSound(GunShot);
	UnloadSound(ReloadSound);
	UnloadMusicStream(bgMusic);
    UnloadTexture(ped);
	UnloadTexture(Background);
    CloseWindow();

    return 0;
}

void Core(){
	if (IsKeyPressed(KEY_P) || IsKeyPressed(KEY_ESCAPE)) {
		gameState = PAUSED;
		return;
	}
	
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) SetX(5);
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) SetX(-5);
	if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(0)) Shoot();
	if (IsKeyPressed(KEY_R) && !isReloading) {
		PlaySound(ReloadSound);
		isReloading = true;
		reloadTimer = 0.0f;
	}

	if(Bullets <= 0 && !isReloading){
		PlaySound(ReloadSound);
		isReloading = true;
		reloadTimer = 0.0f;
	}

	if(isReloading){
		reloadTimer += GetFrameTime();
		if(reloadTimer >= reloadDelay){
			Bullets = MaxBullets;
			isReloading = false;
			reloadTimer = 0.0f;
		}
	}	
    BeginDrawing();
        ClearBackground(RAYWHITE);
		DrawTexturePro(Background, (Rectangle){0, 0, Background.width, Background.height},(Rectangle){0, 0, WIDTH, HEIGHT},(Vector2){0, 0}, 0.0f, WHITE);
		int hudpos = 10;

		#ifdef DEBUG
			DrawText(TextFormat("x = %.2f,y = %.2f",PlayerPos.x,PlayerPos.y), 10, hudpos, 20, BLACK);
			hudpos += 25;
		#endif

		DrawText(TextFormat("Life: %d",Life), 10, hudpos, 20, BLACK);
		if(isReloading){
			DrawText(TextFormat("Reloading .:.:. %.1f", reloadDelay - reloadTimer), 10, hudpos + 25, 20, RED);
		} else {
			DrawText(TextFormat("Bullets: %d",Bullets), 10, hudpos + 25, 20, BLACK);
		}
		DrawText(TextFormat("Kills: %d",Kills), 10, hudpos + 50, 20, BLACK);
		DrawText(TextFormat("Level: %d",Level), 10, hudpos + 75, 20, BLACK);
		for(int i = 0;i<FoeCount;i++){
			UpdateFoe(i);
		}
	for(int i = 0; i < 10; i++){ 
		for(int j = 0;j<FoeCount;j++){
			UpdateBullet(i,j);
		}
	}

	for(int i = 0; i < 5; i++){
		if(Blasting[i]){
			DrawTextureEx(Blast, BlastPos[i], 0.0f, 0.05f, WHITE);
			BlastTimer[i] += GetFrameTime();
			if(BlastTimer[i] >= 2.0f){
				Blasting[i] = false;
				BlastTimer[i] = 0.0f;
			}
		}
	}

        DrawTextureEx(ped, PlayerPos, 0.0f, 0.2f, WHITE);    
		EndDrawing();
}
int SetX(int val){
	int temp = PlayerPos.x + val;
	if(temp < MAX_LEFT || temp > MAX_RIGHT) return 0;
	PlayerPos.x = temp;
	return 1;
}
void UpdateFoe(int foe){
	FoePos[foe].y += 1;
	if(FoePos[foe].y > MAX_FOE_BOTTOM){
		Life -= 1;
		if(Life < 1){
			gameState = GAMEOVER;
			return;
		}
		FoePos[foe].y = FOE_START;
		FoePos[foe].x = RangeRandom(MAX_LEFT,MAX_RIGHT);
	}
	DrawTextureEx(Foe, FoePos[foe], 0.0f, 0.1f, WHITE);
}
void UpdateBullet(int bullet , int foe){
	if(!BulletActive[bullet]) return; 
	
    BulletPos[bullet].y -= 5;

    if(BulletPos[bullet].y < -50){
        BulletActive[bullet] = false;
        return;
    }
    
    DrawTextureEx(BulletTxd, BulletPos[bullet], 0.0f, 0.01f, WHITE);
    
    float bulletRadius = (BulletTxd.width * 0.01f) / 4;
    float foeRadius = (Foe.width * 0.1f) / 8;
    
    Vector2 bulletCenter = {
        BulletPos[bullet].x + (BulletTxd.width * 0.01f / 2), 
        BulletPos[bullet].y + (BulletTxd.height * 0.01f / 2)  
    };
    Vector2 foeCenter = {
        FoePos[foe].x + (Foe.width * 0.1f / 2), 
        FoePos[foe].y + (Foe.height * 0.1f / 2) 
    };

    #ifdef DEBUG
    	DrawCircleLines(foeCenter.x, foeCenter.y, foeRadius, RED);
    	DrawCircleLines(bulletCenter.x, bulletCenter.y, bulletRadius, BLUE);
    #endif

    if(CheckCollisionCircles(bulletCenter, bulletRadius, foeCenter, foeRadius)){
	BlastPos[foe].x = FoePos[foe].x - (Blast.width * 0.05f / 2) + (Foe.width * 0.1f / 2);
	BlastPos[foe].y = FoePos[foe].y - (Blast.height * 0.05f / 2) + (Foe.height * 0.1f / 2);
	Blasting[foe] = true;
	BlastTimer[foe] = 0.0f;        FoePos[foe].y = FOE_START;
        FoePos[foe].x = RangeRandom(MAX_LEFT, MAX_RIGHT);
        BulletActive[bullet] = false;
		Kills += 1;
    }
	if(Kills >= (Level * Level)){
		Level += 1;
		if(Life < 5){
			Life += 1;
		}
		if(Level > 5){
			MaxBullets += 1;
		}
	}
	switch(Level){
		case 2: FoeCount = 2;
				break;
		case 5: FoeCount = 3;
				break;
		case 7: FoeCount = 4;
				break;
		case 10: FoeCount = 5;
				break;
				
	}
}
void Shoot(){
	if(Bullets <= 0) return;
	if(isReloading) return;
	
	for(int i = 0; i < 10; i++){
		if(!BulletActive[i]){
			PlaySound(GunShot);
		    BulletPos[i].x = PlayerPos.x + (ped.width * 0.2f / 2) - (BulletTxd.width * 0.01f / 2);
		    BulletPos[i].y = PlayerPos.y;
			BulletActive[i] = true;
			Bullets -= 1;
			break;
		}
	}
}
void ResetGame() {
	Kills = 0;
	Level = 1;
	Life = 5;
	Bullets = MaxBullets;
	BulletCount = 0;
	FoeCount = 1;
	isReloading = false;
	reloadTimer = 0.0f;
	for(int i = 0; i < 10; i++) BulletActive[i] = false;
	for(int i = 0; i < 5; i++){
		FoePos[i].y = FOE_START;
		FoePos[i].x = RangeRandom(MAX_LEFT,MAX_RIGHT);
		Blasting[i] = false;
		BlastTimer[i] = 0.0f;
	}
	PlayerPos.x = CENTER;
	PlayerPos.y = 330;
}

void DrawMenuScreen() {
	BeginDrawing();
	ClearBackground(RAYWHITE);
	float scaleX = (float)WIDTH / Background.width;
	DrawTextureEx(Background, (Vector2){0, 0}, 0.0f, scaleX, WHITE);

	if(gameState == PAUSED) {
		for(int i = 0; i < FoeCount; i++){
			DrawTextureEx(Foe, FoePos[i], 0.0f, 0.1f, WHITE);
		}
		for(int i = 0; i < 10; i++){
			if(BulletActive[i]) {
				DrawTextureEx(BulletTxd, BulletPos[i], 0.0f, 0.01f, WHITE);
			}
		}
		DrawTextureEx(ped, PlayerPos, 0.0f, 0.2f, WHITE);
	}

	DrawRectangle(0, 0, WIDTH, HEIGHT, Fade(BLACK, 0.7f));
	
	const char* title = (gameState == PAUSED) ? "PAUSED" : GAME_NAME;
	DrawText(title, WIDTH/2 - MeasureText(title, 60)/2, 80, 60, GOLD);

	if(gameState == PAUSED) {
		if (GuiButton((Rectangle){ WIDTH/2 - 100, 200, 200, 50 }, "RESUME")) {
			gameState = PLAYING;
		}
		
		if (GuiButton((Rectangle){ WIDTH/2 - 100, 270, 200, 50 }, "RESTART")) {
			ResetGame();
			gameState = PLAYING;
		}
		
		if (GuiButton((Rectangle){ WIDTH/2 - 100, 340, 200, 50 }, "QUIT")) {
			ResetGame();
			gameState = MENU;
		}
	} else {
		if (GuiButton((Rectangle){ WIDTH/2 - 100, 180, 200, 50 }, "START GAME")) {
			ResetGame();
			gameState = PLAYING;
		}
		
		if (GuiButton((Rectangle){ WIDTH/2 - 100, 250, 200, 50 }, "QUIT")) {
			CloseWindow();
		}
		
		DrawText("CONTROLS:", CENTER, HEIGHT - 120, 30, YELLOW);
		DrawText("<- -> / A - D to Move", CENTER, HEIGHT - 90, 20, WHITE);
		DrawText("SPACE / left click to Shoot", CENTER, HEIGHT - 70, 20, WHITE);
		DrawText("R to Reload, esc/p to menu", CENTER, HEIGHT - 50, 20, WHITE);
	}
	
	EndDrawing();
}

void DrawGameOverScreen() {
	BeginDrawing();
	ClearBackground(BLACK);
	DrawRectangle(0, 0, WIDTH, HEIGHT, Fade(DARKGRAY, 0.9f));
	
	DrawText("GAME OVER!", WIDTH/2 - MeasureText("GAME OVER!", 70)/2, 80, 70, RED);
	
	DrawText(TextFormat("Kills : %d ", Kills), WIDTH/2 - 120, 180, 30, WHITE);
	DrawText(TextFormat("Level: %d", Level), WIDTH/2 - 120, 220, 30, WHITE);
	

	if (GuiButton((Rectangle){ WIDTH/2 - 100, 290, 200, 50 }, "TRY AGAIN")) {
		ResetGame();
		gameState = PLAYING;
	}
	
	if (GuiButton((Rectangle){ WIDTH/2 - 100, 360, 200, 50 }, "MAIN MENU")) {
		ResetGame();
		gameState = MENU;
	}
	
	EndDrawing();
}
