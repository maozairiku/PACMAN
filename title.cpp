//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : 王　ウ華
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "sprite.h"
#include "title.h"
#include "player.h"
#include "ui.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(10)				// テクスチャの数

#define TEXTURE_WIDTH_LOGO			(700)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(350)			// 

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/TitleBg.png",
	"data/TEXTURE/TitleLogo.png",
	"data/TEXTURE/TitleStart.png",
	"data/TEXTURE/TitleQuit.png",
	"data/TEXTURE/TitlePacman.png",
	"data/TEXTURE/TitleGhost.png",
	"data/TEXTURE/TitleAntiGhost.png",
	"data/TEXTURE/effect000.jpg",
};


static bool						g_Use;						// true:使っている  false:未使用
static float					g_w, g_h;					// 幅と高さ
static XMFLOAT3					g_Pos;						// ポリゴンの座標
static int						g_TexNo;					// テクスチャ番号

float	alpha;
bool	flag_alpha;

static BOOL						g_Load = FALSE;


//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
{
	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// 変数の初期化
	g_Use   = true;
	g_w     = TEXTURE_WIDTH;
	g_h     = TEXTURE_HEIGHT;
	g_Pos   = XMFLOAT3(g_w/2, g_h/3, 0.0f);
	g_TexNo = 0;

	alpha = 1.0f;
	flag_alpha = true;

	// BGM再生
	PlaySound(SOUND_LABEL_BGM_TITLE);

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTitle(void)
{
	PLAYER* player = GetPlayer();
	Ui* ui = GetUi();

	// Enter押したら、ステージを切り替える
	if (GetKeyboardTrigger(DIK_RETURN) && ui[0].pos.y == 380)
	{
		player[0].select = GetMode();
		SetFade(FADE_OUT, MODE_TUTORIAL);
	}
	if (GetKeyboardTrigger(DIK_RETURN) && ui[0].pos.y == 475)
	{
		PostQuitMessage(WM_CLOSE);
	}

	// ゲームパッドで入力処理
	//else if (IsButtonTriggered(0, BUTTON_START))
	//{
	//	SetFade(FADE_OUT, MODE_GAME);
	//}
	else if (IsButtonTriggered(0, BUTTON_A) && ui[0].pos.y == 380)
	{
		player[0].select = GetMode();
		SetFade(FADE_OUT, MODE_TUTORIAL);
	}
	else if (IsButtonTriggered(0, BUTTON_A) && ui[0].pos.y == 475)
	{
		PostQuitMessage(WM_CLOSE);
	}

	if (flag_alpha == true)
	{
		alpha -= 0.02f;
		if (alpha <= 0.0f)
		{
			alpha = 0.0f;
			flag_alpha = false;
		}
	}
	else
	{
		alpha += 0.02f;
		if (alpha >= 1.0f)
		{
			alpha = 1.0f;
			flag_alpha = true;
		}
	}






#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);
	
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTitle(void)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	Ui* ui = GetUi();

	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLeftTop(g_VertexBuffer, 0, 0, g_w, g_h, 0.0f, 0.0f, 1.0f, 1.0f);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// タイトルのロゴを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		//SetSprite(g_VertexBuffer, g_Pos.x, g_Pos.y, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f);
		SetSpriteColor(g_VertexBuffer, g_Pos.x, g_Pos.y, TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, 0.0f, 0.0f, 1.0f, 1.0f,
						XMFLOAT4(1.0f, 1.0f, 1.0f, alpha));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}

	// Start
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		float pw = 200;
		if (ui[0].pos.y == 380)
		{
			pw = 250;
		}
		float ph = 80;
		if (ui[0].pos.y == 380)
		{
			ph = 100;
		}

		SetSprite(g_VertexBuffer, 500, 400, pw, ph, 0, 0, 1, 1);

		GetDeviceContext()->Draw(4, 0);
	}

	// Quit
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

		float pw = 200;
		if (ui[0].pos.y == 475)
		{
			pw = 250;
		}
		float ph = 80;
		if (ui[0].pos.y == 475)
		{
			ph = 100;
		}

		SetSprite(g_VertexBuffer, 500, 495, pw, ph, 0, 0, 1, 1);

		GetDeviceContext()->Draw(4, 0);
	}
}

//	// 加減算のテスト
//	SetBlendState(BLEND_MODE_ADD);		// 加算合成
//	SetBlendState(BLEND_MODE_SUBTRACT);	// 減算合成
//	for(int i=0; i<30; i++)
//	{
//		// テクスチャ設定
//		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);
//
//		// １枚のポリゴンの頂点とテクスチャ座標を設定
//		float dx = 100.0f;
//		float dy = 100.0f;
//		float sx = (float)(rand() % 100);
//		float sy = (float)(rand() % 100);
//
//
//		SetSpriteColor(g_VertexBuffer, dx+sx, dy+sy, 50, 50, 0.0f, 0.0f, 1.0f, 1.0f,
//			XMFLOAT4(0.3f, 0.3f, 1.0f, 0.5f));
//
//		// ポリゴン描画
//		GetDeviceContext()->Draw(4, 0);
//	}
// 
//	SetBlendState(BLEND_MODE_ALPHABLEND);	// 半透明処理を元に戻す







