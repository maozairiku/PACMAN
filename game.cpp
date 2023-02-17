//=============================================================================
//
// ゲーム画面処理 [game.cpp]
// Author : 王　ウ華
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "model.h"
#include "game.h"
#include "camera.h"
#include "input.h"
#include "sound.h"
#include "fade.h"
#include "tutorial.h"
#include "ui.h"
#include "dot.h"
#include "explosion.h"
#include "pexplosion.h"
#include "field.h"
#include "sign.h"
#include "rain.h"

#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "sky.h"
#include "shadow.h"
#include "bullet.h"
#include "score.h"
#include "particle.h"
#include "collision.h"
#include "debugproc.h"


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void CheckHit(void);



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static bool	g_bPause = true;	// ポーズON/OFF

static float g_Time;	// Time set (init->最初の時間　/　Update->時間加算　/　g_Time->ずっと加算していますので、initではなく、グローバル変数に設定します。)


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{
	g_Time = 0.0f;

	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	g_bPause = true;

	// フィールドの初期化
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 100, 13.0f, 13.0f);

	// 第二層fieldの初期化
	InitField();

	// camera work init
	InitCamera();

	// ライトを有効化	// 影の初期化処理
	InitShadow();

	// プレイヤーの初期化
	InitPlayer();

	// エネミーの初期化
	InitEnemy();

	// 壁の初期化
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f,  XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 16, 2, 80.0f, 80.0f);

	// 壁(裏側用の半透明)
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_TOP), XMFLOAT3(0.0f,    XM_PI, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_LEFT, 0.0f, 0.0f), XMFLOAT3(0.0f,   XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(MAP_RIGHT, 0.0f, 0.0f), XMFLOAT3(0.0f, -XM_PI * 0.50f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);
	InitMeshWall(XMFLOAT3(0.0f, 0.0f, MAP_DOWN), XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 0.25f), 16, 2, 80.0f, 80.0f);

	// sky
	InitSky();

	// rain
	InitRain();

	// ドットの初期化
	InitDot();

	// サインの初期化
	InitSign();

	// 弾の初期化
	InitBullet();

	// スコアの初期化
	InitScore();

	// explosion
	InitExplosion();

	// パーティクルの初期化
	InitParticle();

	// 爆発パーティクルの初期化
	InitPExplosion();

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_GAMEMODE);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGame(void)
{
	// パーティクルの終了処理
	UninitParticle();

	// skyの終了処理
	UninitSky();

	// rainの終了処理
	UninitRain();

	// スコアの終了処理
	UninitScore();

	// 弾の終了処理
	UninitBullet();

	// ドットの終了処理
	UninitDot();

	// サインの終了処理
	UninitSign();

	// 壁の終了処理
	UninitMeshWall();

	// 地面の終了処理
	UninitMeshField();

	// 第二層fieldの終了処理
	UninitField();

	// エネミーの終了処理
	UninitEnemy();

	// explosion
	UninitExplosion();

	// particle explosion
	UninitPExplosion();

	// プレイヤーの終了処理
	UninitPlayer();

	// 影の終了処理
	UninitShadow();

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGame(void)
{
	// Set time
	SetTime(g_Time);		// g_time 未だは 0.0fとなっています
	g_Time++;				// 加算開始

#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_V))
	{
		g_ViewPortType_Game = (g_ViewPortType_Game + 1) % TYPE_NONE;
		SetViewPort(g_ViewPortType_Game);
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? false : true;
	}


#endif

	if (g_bPause == false)
	{
		// particle explosion
		UpdatePExplosion();

		return;
	}

	// 地面処理の更新
	UpdateMeshField();

	// 第二層fieldの更新
	UpdateField();

	// sky
	UpdateSky();

	// rain
	UpdateRain();

	// プレイヤーの更新処理
	UpdatePlayer();

	// エネミーの更新処理
	UpdateEnemy();

	// 壁処理の更新
	UpdateMeshWall();

	// サインの更新処理
	UpdateSign();

	// 弾の更新処理
	UpdateBullet();

	// ドットの更新処理
	UpdateDot();

	// パーティクルの更新処理
	UpdateParticle();

	// 影の更新処理
	UpdateShadow();

	// 当たり判定処理
	CheckHit();

	// スコアの更新処理
	UpdateScore();

	// set explosion
	UpdateExplosion();

	// particle explosion
	UpdatePExplosion();

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGame0(void)
{
	// 3Dの物を描画する処理
	// 地面の描画処理
	DrawMeshField();

	// 第二層fieldの描画処理
	DrawField();

	// sky
	DrawSky();

	// rain
	DrawRain();

	// 影の描画処理
	DrawShadow();

	// エネミーの描画処理
	DrawEnemy();

	// プレイヤーの描画処理
	DrawPlayer();

	// explosion
	DrawExplosion();

	// 弾の描画処理
	DrawBullet();

	//ドットの描画処理
	DrawDot();

	// 壁の描画処理
	DrawMeshWall();

	// サインの描画処理
	DrawSign();

	// パーティクルの描画処理
	DrawParticle();

	// particle explosion
	DrawPExplosion();

	// 2Dの物を描画する処理
	// Z比較なし
	SetDepthEnable(false);

	// ライティングを無効
	SetLightEnable(false);

	// スコアの描画処理
	DrawScore();


	// ライティングを有効に
	SetLightEnable(true);

	// Z比較あり
	SetDepthEnable(true);
}


void DrawGame(void)
{
	XMFLOAT3 pos;


#ifdef _DEBUG
	// デバッグ表示
	PrintDebugProc("ViewPortType:%d\n", g_ViewPortType_Game);

#endif

	// プレイヤー視点
	pos = GetPlayer()->pos;
	pos.y = 0.0f;			// カメラ酔いを防ぐためにクリアしている
	SetCameraAT(pos);
	SetCamera();

	switch(g_ViewPortType_Game)
	{
	case TYPE_FULL_SCREEN:
		SetViewPort(TYPE_FULL_SCREEN);
		DrawGame0();
		break;

	case TYPE_LEFT_HALF_SCREEN:
	case TYPE_RIGHT_HALF_SCREEN:
		SetViewPort(TYPE_LEFT_HALF_SCREEN);
		DrawGame0();

		// エネミー視点
		//pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_RIGHT_HALF_SCREEN);
		DrawGame0();
		break;

	case TYPE_UP_HALF_SCREEN:
	case TYPE_DOWN_HALF_SCREEN:
		SetViewPort(TYPE_UP_HALF_SCREEN);
		DrawGame0();

		// エネミー視点
		//pos = GetEnemy()->pos;
		pos.y = 0.0f;
		SetCameraAT(pos);
		SetCamera();
		SetViewPort(TYPE_DOWN_HALF_SCREEN);
		DrawGame0();
		break;

	}

}


//=============================================================================
// 当たり判定処理
//=============================================================================
void CheckHit(void)
{
	ENEMY *ghostred = GetGhostRed();		// エネミーのポインターを初期化
	ENEMY* ghostorange = GetGhostOrange();	

	PLAYER *player = GetPlayer();			// プレイヤーのポインターを初期化
	BULLET *bullet = GetBullet();			// 弾のポインターを初期化

	DOT *cookies = GetCookies();
	DOT *hotdog = GetHotdog();
	DOT *cherry = GetCherry();
	DOT *bread = GetBread();
	DOT *croissant = GetCroissant();

	EXPLOSION *explosion = GetExplosion();
	CAMERA *camera = GetCamera();

	// 敵とプレイヤーキャラ
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//敵の有効フラグをチェックする(red)
		if (ghostred[i].use == false)
			continue;

		//BCの当たり判定
		if (CollisionBC(player->pos, ghostred[i].pos, player->size, ghostred[i].size))
		{
			// 当たったから未使用に戻す
			ghostred[i].use = false;
			ReleaseShadow(ghostred[i].shadowIdx);

			// player die
			player->use = false;
			ReleaseShadow(player->shadowIdx);

			// Set Pause
			SetPause(false);

			// camera shake
			SetShake();

			// particle explosion
			SetExplosionParticle();

			// to result
			SetFade(FADE_OUT, MODE_RESULT);
		}


		//敵の有効フラグをチェックする(orange)
		if (ghostorange[i].use == false)
			continue;

		//BCの当たり判定
		if (CollisionBC(player->pos, ghostorange[i].pos, player->size, ghostorange[i].size))
		{
			// 当たったから未使用に戻す
			ghostorange[i].use = false;
			ReleaseShadow(ghostorange[i].shadowIdx);

			// player die
			player->use = false;
			ReleaseShadow(player->shadowIdx);

			// Set Pause
			SetPause(false);

			// camera shake
			SetShake();

			// particle explosion
			SetExplosionParticle();

			// to result
			SetFade(FADE_OUT, MODE_RESULT);
		}
	}

	// プレイヤーとドット
	for (int d = 0; d < MAX_DOT; d++)
	{
		// cookies
		//BCの当たり判定
		if (CollisionBC(player->pos, cookies[d].pos, player->size, cookies[d].size))
		{
			// 食べられた後、座標リセット
			cookies[d].pos = XMFLOAT3(((rand() % 1260) - 650), 12.0f, ((rand() % 1300) - 650));

			//点数加算
			AddScore(150);
		}

		// hotdog
		//BCの当たり判定
		if (CollisionBC(player->pos, hotdog[d].pos, player->size, hotdog[d].size))
		{
			// 食べられた後、座標リセット
			hotdog[d].pos = XMFLOAT3(((rand() % 1260) - 650), 12.0f, ((rand() % 1300) - 650));

			//点数加算
			AddScore(150);
		}

		// cherry
		//BCの当たり判定
		if (CollisionBC(player->pos, cherry[d].pos, player->size, cherry[d].size))
		{
			// 食べられた後、座標リセット
			cherry[d].pos = XMFLOAT3(((rand() % 1260) - 650), 12.0f, ((rand() % 1300) - 650));

			//点数加算
			AddScore(150);
		}

		// bread
		//BCの当たり判定
		if (CollisionBC(player->pos, bread[d].pos, player->size, bread[d].size))
		{
			// 食べられた後、座標リセット
			bread[d].pos = XMFLOAT3(((rand() % 1260) - 650), 12.0f, ((rand() % 1300) - 650));

			//点数加算
			AddScore(150);
		}

		// croissant
		//BCの当たり判定
		if (CollisionBC(player->pos, croissant[d].pos, player->size, croissant[d].size))
		{
			// 食べられた後、座標リセット
			croissant[d].pos = XMFLOAT3(((rand() % 1260) - 650), 12.0f, ((rand() % 1300) - 650));

			//点数加算
			AddScore(150);
		}
	}

	// プレイヤーの弾と敵
	for (int i = 0; i < MAX_BULLET; i++)
	{
		//弾の有効フラグをチェックする
		if (bullet[i].use == false)
			continue;

			//BCの当たり判定
			if (CollisionBC(bullet[i].pos, player->pos, bullet[i].fWidth, player->size))
			{
				// 当たったから未使用に戻す
				bullet[i].use = false;
				ReleaseShadow(bullet[i].shadowIdx);

				// player die
				player->use = false;
				ReleaseShadow(player->shadowIdx);

				// Set Pause
				SetPause(false);

				// camera shake
				SetShake();
				
				// particle explosion
				SetExplosionParticle();
				
				// to result
				SetFade(FADE_OUT, MODE_RESULT);
			}
	}
}

//=============================================================================
// Set Pause
//=============================================================================
void SetPause(BOOL pause)
{
	g_bPause = pause;
}

