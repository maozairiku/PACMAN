//=============================================================================
//
// �p�[�e�B�N������ [pexplosion.cpp]
// Author : ���@�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "pexplosion.h"
#include "player.h"
#include "input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX				(1)			// �e�N�X�`���̐�

#define	PEXPLOSION_SIZE_X		(40.0f)		// ���_�T�C�Y
#define	PEXPLOSION_SIZE_Y		(40.0f)		// ���_�T�C�Y
#define	VALUE_MOVE_PEXPLOSION	(5.0f)		// �ړ����x

#define	MAX_PEXPLOSION			(512)		// �p�[�e�B�N���ő吔

#define	DISP_SHADOW							// �e�̕\��
//#undef DISP_SHADOW

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

} PEXPLOSION;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexPExplosion(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		// ���_�o�b�t�@

static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int						g_TexNo;							// �e�N�X�`���ԍ�

static PEXPLOSION				g_PExplosion[MAX_PEXPLOSION];		// �p�[�e�B�N�����[�N
static XMFLOAT3					g_posBase;							// �r���{�[�h�����ʒu
static float					g_fWidthBase = 5.0f;				// ��̕�
static float					g_fHeightBase = 10.0f;				// ��̍���
static float					g_roty = 0.0f;						// �ړ�����
static float					g_spd = 0.0f;						// �ړ��X�s�[�h

static char* g_TextureName[TEXTURE_MAX] =
{
	"data/TEXTURE/effect000.jpg",
};

static BOOL						g_Load = FALSE;

//=============================================================================
// ����������
//=============================================================================
HRESULT InitPExplosion(void)
{
	// ���_���̍쐬
	MakeVertexPExplosion();

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
	for (int nCntPExplosion = 0; nCntPExplosion < MAX_PEXPLOSION; nCntPExplosion++)
	{
		g_PExplosion[nCntPExplosion].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_PExplosion[nCntPExplosion].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_PExplosion[nCntPExplosion].scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_PExplosion[nCntPExplosion].move = XMFLOAT3(1.0f, 1.0f, 1.0f);

		ZeroMemory(&g_PExplosion[nCntPExplosion].material, sizeof(g_PExplosion[nCntPExplosion].material));
		g_PExplosion[nCntPExplosion].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_PExplosion[nCntPExplosion].fSizeX = PEXPLOSION_SIZE_X;
		g_PExplosion[nCntPExplosion].fSizeY = PEXPLOSION_SIZE_Y;
		g_PExplosion[nCntPExplosion].nIdxShadow = -1;
		g_PExplosion[nCntPExplosion].nLife = 0;
		g_PExplosion[nCntPExplosion].bUse = false;
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
void UninitPExplosion(void)
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
void UpdatePExplosion(void)
{
	PLAYER *pPlayer = GetPlayer();
	g_posBase = pPlayer->pos;

	{
		for (int nCntPExplosion = 0; nCntPExplosion < MAX_PEXPLOSION; nCntPExplosion++)
		{
			if (g_PExplosion[nCntPExplosion].bUse)		// �g�p��
			{
				g_PExplosion[nCntPExplosion].pos.x += g_PExplosion[nCntPExplosion].move.x;
				g_PExplosion[nCntPExplosion].pos.z += g_PExplosion[nCntPExplosion].move.z;

				g_PExplosion[nCntPExplosion].pos.y += g_PExplosion[nCntPExplosion].move.y;
				if (g_PExplosion[nCntPExplosion].pos.y <= g_PExplosion[nCntPExplosion].fSizeY / 2)	// ���n����
				{
					g_PExplosion[nCntPExplosion].pos.y = g_PExplosion[nCntPExplosion].fSizeY / 2;
					g_PExplosion[nCntPExplosion].move.y = -g_PExplosion[nCntPExplosion].move.y * 0.75f;
				}

				g_PExplosion[nCntPExplosion].move.x += (0.0f - g_PExplosion[nCntPExplosion].move.x) * 0.015f;
				g_PExplosion[nCntPExplosion].move.y -= 0.25f;
				g_PExplosion[nCntPExplosion].move.z += (0.0f - g_PExplosion[nCntPExplosion].move.z) * 0.015f;

#ifdef DISP_SHADOW
				if (g_PExplosion[nCntPExplosion].nIdxShadow != -1)		// �e�g�p��
				{
					float colA;

					// �e�̈ʒu�ݒ�
					SetPositionShadow(g_PExplosion[nCntPExplosion].nIdxShadow, XMFLOAT3(g_PExplosion[nCntPExplosion].pos.x, 0.1f, g_PExplosion[nCntPExplosion].pos.z));

					// �e�̐F�̐ݒ�
					colA = g_PExplosion[nCntPExplosion].material.Diffuse.w;
					SetColorShadow(g_PExplosion[nCntPExplosion].nIdxShadow, XMFLOAT4(0.5f, 0.5f, 0.5f, colA));
				}
#endif

				g_PExplosion[nCntPExplosion].nLife--;
				if (g_PExplosion[nCntPExplosion].nLife <= 0)
				{
					g_PExplosion[nCntPExplosion].bUse = false;
					ReleaseShadow(g_PExplosion[nCntPExplosion].nIdxShadow);
					g_PExplosion[nCntPExplosion].nIdxShadow = -1;
				}
				else
				{
					if (g_PExplosion[nCntPExplosion].nLife <= 20)
					{
						g_PExplosion[nCntPExplosion].material.Diffuse.x = 0.7f - (float)(20 - g_PExplosion[nCntPExplosion].nLife) / 20 * 0.7f;
						g_PExplosion[nCntPExplosion].material.Diffuse.y = 0.5f - (float)(20 - g_PExplosion[nCntPExplosion].nLife) / 20 * 0.5f;
						g_PExplosion[nCntPExplosion].material.Diffuse.z = 0.0f - (float)(20 - g_PExplosion[nCntPExplosion].nLife) / 20 * 0.0f;
					}

					if (g_PExplosion[nCntPExplosion].nLife <= 10)
					{
						// ���l�ݒ�
						g_PExplosion[nCntPExplosion].material.Diffuse.w -= 0.05f;
						if (g_PExplosion[nCntPExplosion].material.Diffuse.w < 0.0f)
						{
							g_PExplosion[nCntPExplosion].material.Diffuse.w = 0.0f;
						}
					}
				}
			}
		}
	}
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawPExplosion(void)
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

	for (int nCntPExplosion = 0; nCntPExplosion < MAX_PEXPLOSION; nCntPExplosion++)
	{
		if (g_PExplosion[nCntPExplosion].bUse)
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
			mtxScl = XMMatrixScaling(g_PExplosion[nCntPExplosion].scale.x, g_PExplosion[nCntPExplosion].scale.y, g_PExplosion[nCntPExplosion].scale.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_PExplosion[nCntPExplosion].pos.x, g_PExplosion[nCntPExplosion].pos.y, g_PExplosion[nCntPExplosion].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);

			// �}�e���A���ݒ�
			SetMaterial(g_PExplosion[nCntPExplosion].material);

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
HRESULT MakeVertexPExplosion(void)
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
		vertex[0].Position = XMFLOAT3(-PEXPLOSION_SIZE_X / 2, PEXPLOSION_SIZE_Y / 2, 0.0f);
		vertex[1].Position = XMFLOAT3(PEXPLOSION_SIZE_X / 2, PEXPLOSION_SIZE_Y / 2, 0.0f);
		vertex[2].Position = XMFLOAT3(-PEXPLOSION_SIZE_X / 2, -PEXPLOSION_SIZE_Y / 2, 0.0f);
		vertex[3].Position = XMFLOAT3(PEXPLOSION_SIZE_X / 2, -PEXPLOSION_SIZE_Y / 2, 0.0f);

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
void SetColorPExplosion(int nIdxPExplosion, XMFLOAT4 col)
{
	g_PExplosion[nIdxPExplosion].material.Diffuse = col;
}

//=============================================================================
// �p�[�e�B�N���̔�������
//=============================================================================
int SetPExplosion(XMFLOAT3 pos, XMFLOAT3 move, XMFLOAT4 col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxPExplosion = -1;

	for (int nCntPExplosion = 0; nCntPExplosion < MAX_PEXPLOSION; nCntPExplosion++)
	{
		if (!g_PExplosion[nCntPExplosion].bUse)
		{
			g_PExplosion[nCntPExplosion].pos = pos;
			g_PExplosion[nCntPExplosion].rot = { 0.0f, 0.0f, 0.0f };
			g_PExplosion[nCntPExplosion].scale = { 1.0f, 1.0f, 1.0f };
			g_PExplosion[nCntPExplosion].move = move;
			g_PExplosion[nCntPExplosion].material.Diffuse = col;
			g_PExplosion[nCntPExplosion].fSizeX = fSizeX;
			g_PExplosion[nCntPExplosion].fSizeY = fSizeY;
			g_PExplosion[nCntPExplosion].nLife = nLife;
			g_PExplosion[nCntPExplosion].bUse = true;

			nIdxPExplosion = nCntPExplosion;

#ifdef DISP_SHADOW
			// �e�̐ݒ�
			g_PExplosion[nCntPExplosion].nIdxShadow = CreateShadow(XMFLOAT3(pos.x, 0.1f, pos.z), 0.8f, 0.8f);		// �e�̐ݒ�
			if (g_PExplosion[nCntPExplosion].nIdxShadow != -1)
			{
				SetColorShadow(g_PExplosion[nCntPExplosion].nIdxShadow, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));
			}
#endif

			break;
		}
	}

	return nIdxPExplosion;
}

//=============================================================================
// set particle explosion
//=============================================================================
void SetExplosionParticle(void)
{
	XMFLOAT3 pos;
	XMFLOAT3 move;
	float fAngle, fLength;
	int nLife;
	float fSize;

	pos = g_posBase;

	fAngle = (float)(rand() % 628 - 314) / 100.0f;
	fLength = rand() % (int)(g_fWidthBase * 200) / 100.0f - g_fWidthBase;

	nLife = 30;

	fSize = (float)(rand() % 30 + 20);

	pos.y = fSize / 2;

	for (int i = 0; i < 50; i++)
	{
		move.x = (rand() % 6) - 3;	
		move.y = rand() % 5;		
		move.z = (rand() % 6) - 3;	

		SetPExplosion(pos, move, XMFLOAT4(1.0f, 0.0f, 0.0f, 0.85f), fSize, fSize, nLife);
	}

}