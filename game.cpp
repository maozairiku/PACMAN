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

#include "player.h"
#include "enemy.h"
#include "meshfield.h"
#include "meshwall.h"
#include "sky.h"
#include "shadow.h"
#include "tree.h"
#include "bullet.h"
#include "score.h"
#include "particle.h"
#include "collision.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void CheckHit(void);



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static int	g_ViewPortType_Game = TYPE_FULL_SCREEN;

static bool	g_bPause = true;	// ポーズON/OFF


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGame(void)
{
	g_ViewPortType_Game = TYPE_FULL_SCREEN;

	// フィールドの初期化
	InitMeshField(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), 100, 100, 13.0f, 13.0f);

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

	// 木を生やす
	InitTree();

	// ドットの初期化
	InitDot();

	// 弾の初期化
	InitBullet();

	// スコアの初期化
	InitScore();

	// explosion
	InitExplosion();

	// パーティクルの初期化
	InitParticle();

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

	// sky
	UninitSky();

	// スコアの終了処理
	UninitScore();

	// 弾の終了処理
	UninitBullet();

	// ドットの終了処理
	UninitDot();

	// 木の終了処理
	UninitTree();

	// 壁の終了処理
	UninitMeshWall();

	// 地面の終了処理
	UninitMeshField();

	// エネミーの終了処理
	UninitEnemy();

	// explosion
	UninitExplosion();

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

	if(g_bPause == false)
		return;

	// 地面処理の更新
	UpdateMeshField();

	// sky
	UpdateSky();

	// プレイヤーの更新処理
	UpdatePlayer();

	// エネミーの更新処理
	UpdateEnemy();

	// explosion
	UpdateExplosion();

	// 壁処理の更新
	UpdateMeshWall();

	// 木の更新処理
	UpdateTree();

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
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGame0(void)
{
	// 3Dの物を描画する処理
	// 地面の描画処理
	DrawMeshField();

	// sky
	DrawSky();

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

	// 木の描画処理
	DrawTree();

	// パーティクルの描画処理
	DrawParticle();


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
	//ENEMY* ghostorange = GetGhostOrange();	
	//ENEMY* ghostgreen = GetGhostGreen();	
	//ENEMY* ghostblue = GetGhostBlue();		
	//ENEMY* ghostpurple = GetGhostPurple();	

	PLAYER *player = GetPlayer();			// プレイヤーのポインターを初期化
	BULLET *bullet = GetBullet();			// 弾のポインターを初期化

	DOT *cookies = GetCookies();
	DOT *hotdog = GetHotdog();
	DOT *cherry = GetCherry();
	DOT *bread = GetBread();
	DOT *croissant = GetCroissant();

	EXPLOSION *explosion = GetExplosion();

	// 敵とプレイヤーキャラ
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		//敵の有効フラグをチェックする(red)
		if (ghostred[i].use == false)
			continue;

		//BCの当たり判定
		if (CollisionBC(player->pos, ghostred[i].pos, player->size, ghostred[i].size))
		{
			// 敵キャラクターは倒される
			ghostred[i].use = false;
			ReleaseShadow(ghostred[i].shadowIdx);

			// Game Over
			SetFade(FADE_OUT, MODE_RESULT);
		}


		////敵の有効フラグをチェックする(orange)
		//if (ghostorange[i].use == false)
		//	continue;

		////BCの当たり判定
		//if (CollisionBC(player->pos, ghostorange[i].pos, player->size, ghostorange[i].size))
		//{
		//	// 敵キャラクターは倒される
		//	ghostorange[i].use = false;
		//	ReleaseShadow(ghostorange[i].shadowIdx);

		//	// Game Over
		//	SetFade(FADE_OUT, MODE_RESULT);
		//}

		////敵の有効フラグをチェックする(green)
		//if (ghostgreen[i].use == false)
		//	continue;

		////BCの当たり判定
		//if (CollisionBC(player->pos, ghostgreen[i].pos, player->size, ghostgreen[i].size))
		//{
		//	// 敵キャラクターは倒される
		//	ghostgreen[i].use = false;
		//	ReleaseShadow(ghostgreen[i].shadowIdx);

		//	// Game Over
		//	SetFade(FADE_OUT, MODE_RESULT);
		//}


		////敵の有効フラグをチェックする(blue)
		//if (ghostblue[i].use == false)
		//	continue;

		////BCの当たり判定
		//if (CollisionBC(player->pos, ghostblue[i].pos, player->size, ghostblue[i].size))
		//{
		//	// 敵キャラクターは倒される
		//	ghostblue[i].use = false;
		//	ReleaseShadow(ghostblue[i].shadowIdx);

		//	// Game Over
		//	SetFade(FADE_OUT, MODE_RESULT);
		//}


		////敵の有効フラグをチェックする(purple)
		//if (ghostpurple[i].use == false)
		//	continue;

		////BCの当たり判定
		//if (CollisionBC(player->pos, ghostpurple[i].pos, player->size, ghostpurple[i].size))
		//{
		//	// 敵キャラクターは倒される
		//	ghostpurple[i].use = false;
		//	ReleaseShadow(ghostpurple[i].shadowIdx);

		//	// Game Over
		//	SetFade(FADE_OUT, MODE_RESULT);
		//}

	}

	// プレイヤーとドット
	for (int d = 0; d < MAX_DOT; d++)
	{
		// cookies
		if (cookies[d].use == false)
			continue;

		//BCの当たり判定
		if (CollisionBC(player->pos, cookies[d].pos, player->size, cookies[d].size))
		{
			// ドットは食べられる
			cookies[d].use = false;
			ReleaseShadow(cookies[d].shadowIdx);

			//点数加算
			AddScore(150);
		}


		// hotdog
		if (hotdog[d].use == false)
			continue;

		//BCの当たり判定
		if (CollisionBC(player->pos, hotdog[d].pos, player->size, hotdog[d].size))
		{
			// ドットは食べられる
			hotdog[d].use = false;
			ReleaseShadow(hotdog[d].shadowIdx);

			//点数加算
			AddScore(150);
		}


		// cherry
		if (cherry[d].use == false)
			continue;

		//BCの当たり判定
		if (CollisionBC(player->pos, cherry[d].pos, player->size, cherry[d].size))
		{
			// ドットは食べられる
			cherry[d].use = false;
			ReleaseShadow(cherry[d].shadowIdx);

			//点数加算
			AddScore(150);
		}

		// bread
		if (bread[d].use == false)
			continue;

		//BCの当たり判定
		if (CollisionBC(player->pos, bread[d].pos, player->size, bread[d].size))
		{
			// ドットは食べられる
			bread[d].use = false;
			ReleaseShadow(bread[d].shadowIdx);

			//点数加算
			AddScore(150);
		}

		// croissant
		if (croissant[d].use == false)
			continue;

		//BCの当たり判定
		if (CollisionBC(player->pos, croissant[d].pos, player->size, croissant[d].size))
		{
			// ドットは食べられる
			croissant[d].use = false;
			ReleaseShadow(croissant[d].shadowIdx);

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

				// explosion
				SetExplosion(XMFLOAT3(player->pos.x, -15.0f, player->pos.z), 50.0f, 50.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ANIM_WAIT = 24 flame
				if (explosion[i].countAnim >= 24 && player->use == false)
				{
					explosion[i].bUse = false;
				}

				// to result
				//SetFade(FADE_OUT, MODE_RESULT);
				
			}
	}



	// dotが全部なくなったら状態遷移
	int dot_count = 0;
	for (int i = 0; i < MAX_DOT; i++)
	{
		if (cookies[i].use == false) continue;
		dot_count++;
	}

	// dotが０匹？
	if (dot_count == 0)
	{
		SetFade(FADE_OUT, MODE_RESULT);
	}

}


