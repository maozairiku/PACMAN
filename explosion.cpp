//=============================================================================
//
// 爆発処理 [explosion.cpp]
// Author : 王　ウ華
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "explosion.h"
#include "explosion.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX				(2)				// テクスチャの数

#define	EXPLOSION_WIDTH			(50.0f)			// 頂点サイズ
#define	EXPLOSION_HEIGHT		(50.0f)			// 頂点サイズ

#define ANIM_WAIT				(24)			// アニメーションの切り替わるWait値
#define ANIM_WAIT2				(30)			//

#define	MAX_EXPLOSION			(256)			// 爆発最大数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexExplosion(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static EXPLOSION					g_Explosion[MAX_EXPLOSION];	// 爆発ワーク
static int					g_TexNo;			// テクスチャ番号
static bool					g_bAlpaTest;		// アルファテストON/OFF

static char* g_TextureName[] =
{
	"data/TEXTURE/explosion.png",
	"data/TEXTURE/explosion2.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitExplosion(void)
{
	MakeVertexExplosion();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	// 爆発ワークの初期化
	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{
		ZeroMemory(&g_Explosion[nCntExplosion].material, sizeof(g_Explosion[nCntExplosion].material));
		g_Explosion[nCntExplosion].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Explosion[nCntExplosion].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Explosion[nCntExplosion].scl = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Explosion[nCntExplosion].fWidth = EXPLOSION_WIDTH;
		g_Explosion[nCntExplosion].fHeight = EXPLOSION_HEIGHT;
		g_Explosion[nCntExplosion].bUse = false;
		g_Explosion[nCntExplosion].countAnim = 0.0f;
		g_Explosion[nCntExplosion].animFlame = 0.0f;
		g_Explosion[nCntExplosion].type = DIE_EXPLO;
	}

	g_bAlpaTest = true;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitExplosion(void)
{
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// テクスチャの解放
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateExplosion(void)
{	
	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{
		if (g_Explosion[nCntExplosion].bUse == true)
		{
			D3D11_MAPPED_SUBRESOURCE msr;
			VERTEX_3D* vertex;
			
			float fWidth = g_Explosion[nCntExplosion].fWidth;
			float fHeight = g_Explosion[nCntExplosion].fHeight;
			
			switch (g_Explosion[nCntExplosion].type)
			{
			case DIE_EXPLO:
				g_Explosion[nCntExplosion].countAnim += 1.0f;
				if ((int)g_Explosion[nCntExplosion].countAnim % 3 == 0)
				{
					g_Explosion[nCntExplosion].animFlame++;
				}

				// 頂点バッファに値をセットする
				GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

				vertex = (VERTEX_3D*)msr.pData;

				// 頂点座標の設定
				vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
				vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
				vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
				vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

				// 拡散光の設定
				vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

				// テクスチャ座標の設定
				vertex[0].TexCoord = XMFLOAT2(g_Explosion[nCntExplosion].animFlame / 8, 0.0f);
				vertex[1].TexCoord = XMFLOAT2((g_Explosion[nCntExplosion].animFlame + 1) / 8, 0.0f);
				vertex[2].TexCoord = XMFLOAT2(g_Explosion[nCntExplosion].animFlame / 8, 1.0f);
				vertex[3].TexCoord = XMFLOAT2((g_Explosion[nCntExplosion].animFlame + 1) / 8, 1.0f);

				GetDeviceContext()->Unmap(g_VertexBuffer, 0);

				if (g_Explosion[nCntExplosion].countAnim >= ANIM_WAIT)
				{
					g_Explosion[nCntExplosion].bUse = false;
					ReleaseShadow(g_Explosion[nCntExplosion].nIdxShadow);
					g_Explosion[nCntExplosion].countAnim = 0.0f;
				}
				break;

			case OFFGROUND_EXPLO:
				g_Explosion[nCntExplosion].countAnim += 1.0f;
				if ((int)g_Explosion[nCntExplosion].countAnim % 3 == 0)
				{
					g_Explosion[nCntExplosion].animFlame++;
				}

				// 頂点バッファに値をセットする
				GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

				vertex = (VERTEX_3D*)msr.pData;

				// 頂点座標の設定
				vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
				vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
				vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
				vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

				// 拡散光の設定
				vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

				// テクスチャ座標の設定
				vertex[0].TexCoord = XMFLOAT2(g_Explosion[nCntExplosion].animFlame / 10, 0.0f);
				vertex[1].TexCoord = XMFLOAT2((g_Explosion[nCntExplosion].animFlame + 1) / 10, 0.0f);
				vertex[2].TexCoord = XMFLOAT2(g_Explosion[nCntExplosion].animFlame / 10, 1.0f);
				vertex[3].TexCoord = XMFLOAT2((g_Explosion[nCntExplosion].animFlame + 1) / 10, 1.0f);

				GetDeviceContext()->Unmap(g_VertexBuffer, 0);

				if (g_Explosion[nCntExplosion].countAnim >= ANIM_WAIT2)
				{
					g_Explosion[nCntExplosion].bUse = false;
					ReleaseShadow(g_Explosion[nCntExplosion].nIdxShadow);
					g_Explosion[nCntExplosion].countAnim = 0.0f;
				}
				break;
			}
		}
	}


#ifdef _DEBUG
	// アルファテストON/OFF
	if (GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? false : true;
	}

	//// アルファテストの閾値変更
	//if(GetKeyboardPress(DIK_I))
	//{
	//	g_nAlpha--;
	//	if(g_nAlpha < 0)
	//	{
	//		g_nAlpha = 0;
	//	}
	//}
	//if(GetKeyboardPress(DIK_K))
	//{
	//	g_nAlpha++;
	//	if(g_nAlpha > 255)
	//	{
	//		g_nAlpha = 255;
	//	}
	//}
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawExplosion(void)
{
	// αテスト設定
	if (g_bAlpaTest == true)
	{
		// αテストを有効に
		SetAlphaTestEnable(true);
	}

	// ライティングを無効
	SetLightEnable(false);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA* cam = GetCamera();

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_EXPLOSION; i++)
	{
		if (g_Explosion[i].bUse)
		{

			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			//mtxWorld = XMMatrixInverse(nullptr, mtxView);
			//mtxWorld.r[3].m128_f32[0] = 0.0f;
			//mtxWorld.r[3].m128_f32[1] = 0.0f;
			//mtxWorld.r[3].m128_f32[2] = 0.0f;

			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// スケールを反映
			mtxScl = XMMatrixScaling(g_Explosion[i].scl.x, g_Explosion[i].scl.y, g_Explosion[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Explosion[i].pos.x, g_Explosion[i].pos.y, g_Explosion[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);


			// マテリアル設定
			SetMaterial(g_Explosion[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Explosion[i].type]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(true);

	// αテストを無効に
	SetAlphaTestEnable(false);
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexExplosion(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = EXPLOSION_WIDTH;
	float fHeight = EXPLOSION_HEIGHT;

	// 頂点座標の設定
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// 拡散光の設定
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(0.125f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(0.125f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 爆発のパラメータをセット
//=============================================================================
int SetExplosion(XMFLOAT3 pos, float fWidth, float fHeight, int Type)
{
	int nIdxExplosion = -1;

	for (int nCntExplosion = 0; nCntExplosion < MAX_EXPLOSION; nCntExplosion++)
	{
		if (!g_Explosion[nCntExplosion].bUse)
		{
			if (Type == DIE_EXPLO)
			{
				g_Explosion[nCntExplosion].pos = pos;
			}
			else if(Type == OFFGROUND_EXPLO)
			{
				g_Explosion[nCntExplosion].pos = XMFLOAT3(pos.x, 0.0, pos.z);
			}
			g_Explosion[nCntExplosion].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_Explosion[nCntExplosion].fWidth = fWidth;
			g_Explosion[nCntExplosion].fHeight = fHeight;
			g_Explosion[nCntExplosion].type = Type;
			g_Explosion[nCntExplosion].bUse = true;

			// 影の設定
			g_Explosion[nCntExplosion].nIdxShadow = CreateShadow(g_Explosion[nCntExplosion].pos, 0.5f, 0.5f);

			nIdxExplosion = nCntExplosion;

			break;
		}
	}

	return nIdxExplosion;
}

//=============================================================================
// explosionの取得
//=============================================================================
EXPLOSION* GetExplosion()
{
	return &g_Explosion[0];
}

