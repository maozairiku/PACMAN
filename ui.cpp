//=============================================================================
//
// UI処理 [ui.cpp]
// Author : 王　ウ華
//
//=============================================================================
#include "main.h"
#include "ui.h"
#include "player.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(8000)			// (SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(1080)			// (SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(4)				// テクスチャの数

#define UI_ITEM_MAX					(3)				// UIの数



//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/UI/heart.png",
	"data/TEXTURE/bar_white.png",
};


static BOOL		g_Load = FALSE;		// 初期化を行ったかのフラグ
static Ui		g_UI[UI_ITEM_MAX];

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitUi(void)
{
	ID3D11Device* pDevice = GetDevice();

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
	for (int i = 0; i < UI_ITEM_MAX; i++)
	{
		g_UI[i].use = FALSE;

		g_UI[i].w = TEXTURE_WIDTH;
		g_UI[i].h = TEXTURE_HEIGHT;
		g_UI[i].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_UI[i].texNo = 0;

		g_UI[i].scrl = 0.0f;		// TEXスクロール
		g_UI[i].scrl2 = 0.0f;		// TEXスクロール
	}

	switch (GetMode())
	{
	case MODE_TITLE:

		g_UI[0].use = true;
		g_UI[0].texNo = 2;
		g_UI[0].pos = XMFLOAT3(340.0f, 380.0f, 0.0f);
		g_UI[0].w = 50;
		g_UI[0].h = 50;

		break;
	}

	g_Load = TRUE;
	return S_OK;
}




//=============================================================================
// 終了処理
//=============================================================================
void UninitUi(void)
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
void UpdateUi(void)
{
	for (int i = 0; i < UI_ITEM_MAX; i++)
	{
		if (g_UI[i].use)
		{
			g_UI[i].old_pos = g_UI[i].pos;	// １フレ前の情報を保存

			switch (GetMode())
			{
			case MODE_TITLE:

				if (GetKeyboardTrigger(DIK_DOWN))
				{
					g_UI[i].pos.y += 95;
				}

				if (GetKeyboardTrigger(DIK_UP))
				{
					g_UI[i].pos.y -= 95;
				}

				if (g_UI[i].pos.y > 475)
				{
					g_UI[i].pos.y = 380;
				}

				if (g_UI[i].pos.y < 380)
				{
					g_UI[i].pos.y = 475;
				}

				break;
			}
		}
	}


#ifdef _DEBUG	// デバッグ情報を表示する

#endif

}




//=============================================================================
// 描画処理
//=============================================================================
void DrawUi(void)
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

	PLAYER* player = GetPlayer();

#ifdef _DEBUG	// デバッグ情報を表示する

#endif
}

Ui* GetUi(void)
{
	return &g_UI[0];
}
