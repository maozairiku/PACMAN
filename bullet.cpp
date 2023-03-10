//=============================================================================
//
// 弾発射処理 [bullet.cpp]
// Author : 王　ウ華
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "bullet.h"
#include "sound.h"
#include "explosion.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(2)				// テクスチャの数

#define	BULLET_WIDTH		(20.0f)			// 頂点サイズ
#define	BULLET_HEIGHT		(20.0f)			// 頂点サイズ

#define	BULLET_SPEED		(5.0f)			// 弾の移動スピード


//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexBullet(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static BULLET						g_Bullet[MAX_BULLET];	// バレットワーク
static int							g_TexNo;				// テクスチャ番号
static BOOL							g_Load = FALSE;

static float						g_frameCounter = 0.0f; // フレームカウンター

static char *g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/bullet000.png",
	"data/TEXTURE/Simple007.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet(void)
{
	MakeVertexBullet();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 1;

	// 弾ワークの初期化
	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{
		ZeroMemory(&g_Bullet[nCntBullet].material, sizeof(g_Bullet[nCntBullet].material));
		g_Bullet[nCntBullet].material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

		g_Bullet[nCntBullet].pos = { 0.0f, 0.0f, 0.0f };
		g_Bullet[nCntBullet].rot = { 0.0f, 0.0f, 0.0f };
		g_Bullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
		g_Bullet[nCntBullet].spd = BULLET_SPEED;
		g_Bullet[nCntBullet].fWidth = BULLET_WIDTH;
		g_Bullet[nCntBullet].fHeight = BULLET_HEIGHT;
		g_Bullet[nCntBullet].moveType = LINE;
		g_Bullet[nCntBullet].aniFrame = 0;
		g_Bullet[nCntBullet].aniDir = 1;
		g_Bullet[nCntBullet].beizerPos[0] = { 0.0f, 0.0f, 0.0f };
		g_Bullet[nCntBullet].beizerPos[1] = { 0.0f, 0.0f, 0.0f };
		g_Bullet[nCntBullet].beizerPos[2] = { 0.0f, 0.0f, 0.0f };
		g_Bullet[nCntBullet].beizerPos[3] = { 0.0f, 0.0f, 0.0f };
		g_Bullet[nCntBullet].beizerTime = 0.0f;
		g_Bullet[nCntBullet].use = false;
	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBullet(void)
{
	if (g_Load == FALSE) return;

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

	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBullet(void)
{

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_Bullet[i].use)
		{
			// アニメーション処理
			// 頂点バッファに値をセットする
			D3D11_MAPPED_SUBRESOURCE msr;
			GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

			VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

			float fWidth = BULLET_WIDTH;
			float fHeight = BULLET_HEIGHT;

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
			vertex[0].TexCoord = XMFLOAT2(g_Bullet[i].aniFrame / 5.0f, 0.0f);
			vertex[1].TexCoord = XMFLOAT2((g_Bullet[i].aniFrame + 1) / 5.0f, 0.0f);
			vertex[2].TexCoord = XMFLOAT2(g_Bullet[i].aniFrame / 5.0f, 0.5f);
			vertex[3].TexCoord = XMFLOAT2((g_Bullet[i].aniFrame + 1) / 5.0f, 0.5f);

			GetDeviceContext()->Unmap(g_VertexBuffer, 0);

			if (g_Bullet[i].aniFrame == 4)
			{
				g_Bullet[i].aniDir = -1;
			}
			else if (g_Bullet[i].aniFrame == 0)
			{
				g_Bullet[i].aniDir = 1;
			}
			
			g_Bullet[i].aniFrame += g_Bullet[i].aniDir;
			
			// 弾の移動処理
			switch (g_Bullet[i].moveType)
			{
			case LINE:
				g_Bullet[i].pos.x -= sinf(g_Bullet[i].rot.y) * g_Bullet[i].spd;
				g_Bullet[i].pos.z -= cosf(g_Bullet[i].rot.y) * g_Bullet[i].spd;
				break;

			case BEZIER:
				g_Bullet[i].pos.x = powf(1.0f - g_Bullet[i].beizerTime, 3.0f) * g_Bullet[i].beizerPos[0].x + 3.0f * powf(1.0f - g_Bullet[i].beizerTime, 2.0f) * g_Bullet[i].beizerTime * g_Bullet[i].beizerPos[1].x + 3.0f * (1.0f - g_Bullet[i].beizerTime) * powf(g_Bullet[i].beizerTime, 2.0f) * g_Bullet[i].beizerPos[2].x + powf(g_Bullet[i].beizerTime, 3.0f) * g_Bullet[i].beizerPos[3].x;
				g_Bullet[i].pos.y = powf(1.0f - g_Bullet[i].beizerTime, 3.0f) * g_Bullet[i].beizerPos[0].y + 3.0f * powf(1.0f - g_Bullet[i].beizerTime, 2.0f) * g_Bullet[i].beizerTime * g_Bullet[i].beizerPos[1].y + 3.0f * (1.0f - g_Bullet[i].beizerTime) * powf(g_Bullet[i].beizerTime, 2.0f) * g_Bullet[i].beizerPos[2].y + powf(g_Bullet[i].beizerTime, 3.0f) * g_Bullet[i].beizerPos[3].y;
				g_Bullet[i].pos.z = powf(1.0f - g_Bullet[i].beizerTime, 3.0f) * g_Bullet[i].beizerPos[0].z + 3.0f * powf(1.0f - g_Bullet[i].beizerTime, 2.0f) * g_Bullet[i].beizerTime * g_Bullet[i].beizerPos[1].z + 3.0f * (1.0f - g_Bullet[i].beizerTime) * powf(g_Bullet[i].beizerTime, 2.0f) * g_Bullet[i].beizerPos[2].z + powf(g_Bullet[i].beizerTime, 3.0f) * g_Bullet[i].beizerPos[3].z;
				g_Bullet[i].beizerTime += 0.01f;
				break;
			}
			

			// 影の位置設定
			SetPositionShadow(g_Bullet[i].shadowIdx, XMFLOAT3(g_Bullet[i].pos.x, 0.1f, g_Bullet[i].pos.z));


			// フィールドの外に出たら弾を消す処理
			if (g_Bullet[i].pos.x < MAP_LEFT
				|| g_Bullet[i].pos.x > MAP_RIGHT
				|| g_Bullet[i].pos.z < MAP_DOWN
				|| g_Bullet[i].pos.z > MAP_TOP)
			{
				g_Bullet[i].use = false;
				ReleaseShadow(g_Bullet[i].shadowIdx);
			}
			
			if (g_Bullet[i].beizerTime > 1.0f)
			{
				g_Bullet[i].beizerTime = 0.0f;
				g_Bullet[i].use = false;
				SetExplosion(g_Bullet[i].pos, 50.0f, 50.0f, OFFGROUND_EXPLO);
				ReleaseShadow(g_Bullet[i].shadowIdx);
			}

		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBullet(void)
{
	// ライティングを無効
	SetLightEnable(false);

	SetAlphaTestEnable(true);

	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld, mtxView;
	CAMERA* cam = GetCamera();

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_Bullet[i].use)
		{
			// ワールドマトリックスの初期化
			mtxWorld = XMMatrixIdentity();

			// ビューマトリックスを取得
			mtxView = XMLoadFloat4x4(&cam->mtxView);

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
			mtxScl = XMMatrixScaling(g_Bullet[i].scl.x, g_Bullet[i].scl.y, g_Bullet[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// 移動を反映
			mtxTranslate = XMMatrixTranslation(g_Bullet[i].pos.x, g_Bullet[i].pos.y, g_Bullet[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&mtxWorld);

			XMStoreFloat4x4(&g_Bullet[i].mtxWorld, mtxWorld);


			// マテリアル設定
			SetMaterial(g_Bullet[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(true);
	SetAlphaTestEnable(false);
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexBullet(void)
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

	float fWidth = BULLET_WIDTH;
	float fHeight = BULLET_HEIGHT;

	// 頂点座標の設定
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// 拡散光の設定
	vertex[0].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[1].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[2].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	vertex[3].Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };

	// テクスチャ座標の設定
	vertex[0].TexCoord = { 0.0f, 0.0f };
	vertex[1].TexCoord = { 1.0f, 0.0f };
	vertex[2].TexCoord = { 0.0f, 1.0f };
	vertex[3].TexCoord = { 1.0f, 1.0f };

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 弾のパラメータをセット
//=============================================================================
int SetBullet(XMFLOAT3 pos, XMFLOAT3 rot)
{
	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{
		if (!g_Bullet[nCntBullet].use)
		{
			g_Bullet[nCntBullet].pos = pos;
			g_Bullet[nCntBullet].rot = rot;
			g_Bullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
			g_Bullet[nCntBullet].moveType = LINE;
			g_Bullet[nCntBullet].use = true;

			// 影の設定
			g_Bullet[nCntBullet].shadowIdx = CreateShadow(g_Bullet[nCntBullet].pos, 0.5f, 0.5f);

			nIdxBullet = nCntBullet;

			PlaySound(SOUND_LABEL_SE_shot000);

			break;
		}
	}

	return nIdxBullet;
}

int SetBulletBezier(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 targetPos)
{
	int nIdxBullet = -1;

	for (int nCntBullet = 0; nCntBullet < MAX_BULLET; nCntBullet++)
	{
		if (!g_Bullet[nCntBullet].use)
		{
			float length = sqrtf(powf(targetPos.x - pos.x, 2.0f) + powf(targetPos.y - pos.y, 2.0f) + powf(targetPos.z - pos.z, 2.0f));
			g_Bullet[nCntBullet].pos = pos;
			g_Bullet[nCntBullet].rot = rot;
			g_Bullet[nCntBullet].scl = { 1.0f, 1.0f, 1.0f };
			g_Bullet[nCntBullet].moveType = BEZIER;
			g_Bullet[nCntBullet].beizerPos[0] = pos;
			g_Bullet[nCntBullet].beizerPos[1] = { pos.x - sinf(rot.y) * (length / 5), pos.y + 100.0f, pos.z - cosf(rot.y) * (length / 5) };
			g_Bullet[nCntBullet].beizerPos[2] = { targetPos.x + sinf(rot.y) * (length / 5), targetPos.y + 50.0f, targetPos.z + cosf(rot.y) * (length / 5) };
			g_Bullet[nCntBullet].beizerPos[3] = targetPos;
			g_Bullet[nCntBullet].beizerTime = 0.0f;
			g_Bullet[nCntBullet].use = true;

			// 影の設定
			g_Bullet[nCntBullet].shadowIdx = CreateShadow(g_Bullet[nCntBullet].pos, 0.5f, 0.5f);

			nIdxBullet = nCntBullet;

			PlaySound(SOUND_LABEL_SE_shot000);

			break;
		}
	}

	return nIdxBullet;
}

//=============================================================================
// 弾の取得
//=============================================================================
BULLET *GetBullet(void)
{
	return &(g_Bullet[0]);
}

