//=============================================================================
//
// �p�[�e�B�N������ [rain.cpp]
// Author : ���E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "rain.h"
#include "player.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(1)			// �e�N�X�`���̐�

#define	RAIN_SIZE_X			(2.0f)		// ���_�T�C�Y
#define	RAIN_SIZE_Y			(5.0f)		// ���_�T�C�Y
#define	VALUE_MOVE_RAIN		(5.0f)		// �ړ����x

#define	MAX_RAIN			(1024)		// �p�[�e�B�N���ő吔

#define	DISP_SHADOW						// �e�̕\��

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT3		pos;			// �ʒu
	XMFLOAT3		rot;			// ��]
	XMFLOAT3		scale;			// �X�P�[��
	XMFLOAT3		move;			// �ړ���
	MATERIAL		material;		// �}�e���A��
	float			fSizeX;			// ��
	float			fSizeY;			// ����
	int				nIdxShadow;		// �eID
	int				nLife;			// ����
	bool			bUse;			// �g�p���Ă��邩�ǂ���

} RAIN;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexRain(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_�o�b�t�@

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							g_TexNo;					// �e�N�X�`���ԍ�

static RAIN						g_Rain[MAX_RAIN];				// �p�[�e�B�N�����[�N
static XMFLOAT3					g_posBase;						// �r���{�[�h�����ʒu
static float					g_fWidthBase = 5.0f;			// ��̕�
static float					g_fHeightBase = 10.0f;			// ��̍���
static float					g_roty = 0.0f;					// �ړ�����
static float					g_spd = 0.0f;					// �ړ��X�s�[�h

static char* g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/effect000.jpg",
};

static BOOL						g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitRain(void)
{
	// ���_���̍쐬
	MakeVertexRain();

	// �e�N�X�`������
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

	g_TexNo = 0;

	// �p�[�e�B�N�����[�N�̏�����
	for (int nCntRain = 0; nCntRain < MAX_RAIN; nCntRain++)
	{
		g_Rain[nCntRain].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Rain[nCntRain].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Rain[nCntRain].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_Rain[nCntRain].move = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_Rain[nCntRain].material, sizeof(g_Rain[nCntRain].material));
		g_Rain[nCntRain].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_Rain[nCntRain].fSizeX = RAIN_SIZE_X;
		g_Rain[nCntRain].fSizeY = RAIN_SIZE_Y;
		g_Rain[nCntRain].nIdxShadow = -1;
		g_Rain[nCntRain].nLife = 0;
		g_Rain[nCntRain].bUse = false;
	}

	g_posBase = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_roty = 0.0f;
	g_spd = 0.0f;

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitRain(void)
{
	if (g_Load == FALSE) return;

	//�e�N�X�`���̉��
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateRain(void)
{

	{
		for (int nCntRain = 0; nCntRain < MAX_RAIN; nCntRain++)
		{
			if (g_Rain[nCntRain].bUse)				// �g�p��
			{
				g_Rain[nCntRain].pos.x += g_Rain[nCntRain].move.x;
				g_Rain[nCntRain].pos.y += g_Rain[nCntRain].move.y;

				if (g_Rain[nCntRain].pos.y < 1.0f)	// ���n����  
				{
					g_Rain[nCntRain].bUse = FALSE;
				}


#ifdef DISP_SHADOW		// shadow�p

#endif

				g_Rain[nCntRain].nLife--;
				if (g_Rain[nCntRain].nLife <= 0)
				{
					g_Rain[nCntRain].bUse = false;
					ReleaseShadow(g_Rain[nCntRain].nIdxShadow);
					g_Rain[nCntRain].nIdxShadow = -1;
				}
				else
				{
					if (g_Rain[nCntRain].nLife <= 80)
					{
						g_Rain[nCntRain].material.Diffuse.x = 1.0f - (float)(80 - g_Rain[nCntRain].nLife) / 80 * 1.0f;
						g_Rain[nCntRain].material.Diffuse.y = 1.0f - (float)(80 - g_Rain[nCntRain].nLife) / 80 * 1.0f;
						g_Rain[nCntRain].material.Diffuse.z = 1.0f - (float)(80 - g_Rain[nCntRain].nLife) / 80 * 1.0f;
					}

					if (g_Rain[nCntRain].nLife <= 20)
					{
						// ���l�ݒ�
						g_Rain[nCntRain].material.Diffuse.w -= 0.05f;
						if (g_Rain[nCntRain].material.Diffuse.w < 0.0f)
						{
							g_Rain[nCntRain].material.Diffuse.w = 0.0f;
						}
					}
				}
			}
		}

		// �p�[�e�B�N������
		{
			XMFLOAT3 move;
			float fAngle, fLength;
			int nLife;
			float fSize;

			// rain set
			for (int r = 0; r < 100; r++)
			{
				fAngle = (float)(rand() % 628 - 314) / 100.0f;
				fLength = rand() % (int)(g_fWidthBase * 200) / 100.0f - g_fWidthBase;
				move.x = 0.0f;
				move.y = -(rand() % 10);
				move.z = 0.0f;

				nLife = rand() % 100 + 150;

				// �r���{�[�h�̐ݒ�
				SetRain(XMFLOAT3((rand() % 1300 - 650), 100.0f, (rand() % 1300 - 650)), move, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.85f), RAIN_SIZE_X, RAIN_SIZE_Y, nLife);
			}
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawRain(void)
{
	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA* cam = GetCamera();

	// ���C�e�B���O�𖳌���
	SetLightEnable(false);

	// ���Z�����ɐݒ�
	SetBlendState(BLEND_MODE_ADD);

	// Z��r����
	SetDepthEnable(false);

	// �t�H�O����
	SetFogEnable(false);

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �e�N�X�`���ݒ�
	GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

	for (int nCntRain = 0; nCntRain < MAX_RAIN; nCntRain++)
	{
		if (g_Rain[nCntRain].bUse)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
			mtxView = XMLoadFloat4x4(&cam->mtxView);

			// ������������������
			mtxWorld.r[0].m128_f32[0] = mtxView.r[0].m128_f32[0];
			mtxWorld.r[0].m128_f32[1] = mtxView.r[1].m128_f32[0];
			mtxWorld.r[0].m128_f32[2] = mtxView.r[2].m128_f32[0];

			mtxWorld.r[1].m128_f32[0] = mtxView.r[0].m128_f32[1];
			mtxWorld.r[1].m128_f32[1] = mtxView.r[1].m128_f32[1];
			mtxWorld.r[1].m128_f32[2] = mtxView.r[2].m128_f32[1];

			mtxWorld.r[2].m128_f32[0] = mtxView.r[0].m128_f32[2];
			mtxWorld.r[2].m128_f32[1] = mtxView.r[1].m128_f32[2];
			mtxWorld.r[2].m128_f32[2] = mtxView.r[2].m128_f32[2];

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_Rain[nCntRain].scale.x, g_Rain[nCntRain].scale.y, g_Rain[nCntRain].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_Rain[nCntRain].pos.x, g_Rain[nCntRain].pos.y, g_Rain[nCntRain].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_Rain[nCntRain].material);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(true);

	// �ʏ�u�����h�ɖ߂�
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z��r�L��
	SetDepthEnable(true);

	// �t�H�O�L��
	SetFogEnable(true);

}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexRain(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//���_�o�b�t�@�̒��g�𖄂߂�
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// ���_���W�̐ݒ�
		vertex[0].Position = XMFLOAT3(-RAIN_SIZE_X / 2, RAIN_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(RAIN_SIZE_X / 2, RAIN_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-RAIN_SIZE_X / 2, -RAIN_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(RAIN_SIZE_X / 2, -RAIN_SIZE_Y / 2, 0.0f);

		// �@���̐ݒ�
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

		// ���ˌ��̐ݒ�
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		vertex[0].TexCoord = { 0.0f, 0.0f };
		vertex[1].TexCoord = { 1.0f, 0.0f };
		vertex[2].TexCoord = { 0.0f, 1.0f };
		vertex[3].TexCoord = { 1.0f, 1.0f };

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// �}�e���A���J���[�̐ݒ�
//=============================================================================
void SetColorRain(int nIdxRain, XMFLOAT4 col)
{
	g_Rain[nIdxRain].material.Diffuse = col;
}

//=============================================================================
// �p�[�e�B�N���̔�������
//=============================================================================
int SetRain(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxRain = -1;

	for (int nCntRain = 0; nCntRain < MAX_RAIN; nCntRain++)
	{
		if (!g_Rain[nCntRain].bUse)
		{
			g_Rain[nCntRain].pos = pos;
			g_Rain[nCntRain].rot = { 0.0f, 0.0f, 0.0f };
			g_Rain[nCntRain].scale = { 1.0f, 1.0f, 1.0f };
			g_Rain[nCntRain].move = move;
			g_Rain[nCntRain].material.Diffuse = col;
			g_Rain[nCntRain].fSizeX = fSizeX;
			g_Rain[nCntRain].fSizeY = fSizeY;
			g_Rain[nCntRain].nLife = nLife;
			g_Rain[nCntRain].bUse = true;

			nIdxRain = nCntRain;

#ifdef DISP_SHADOW		// shadow�p

#endif

			break;
		}
	}

	return nIdxRain;
}
