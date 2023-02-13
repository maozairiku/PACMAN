//=============================================================================
//
// �T�C������ [sign.cpp]
// Author : ���@�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "sign.h"
#include "dot.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(5)				// �e�N�X�`���̐�

#define	SIGN_WIDTH			(50.0f)			// ���_�T�C�Y
#define	SIGN_HEIGHT			(80.0f)			// ���_�T�C�Y

#define SIGN_OFFSET_Y		(15.0f)			// ����

#define	MAX_SIGN			(256)			// �ő吔


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexSign(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static SIGN			g_cookiesign[MAX_SIGN];
static SIGN			g_hotdogsign[MAX_SIGN];
static SIGN			g_cherrysign[MAX_SIGN];
static SIGN			g_breadsign[MAX_SIGN];
static SIGN			g_croissantsign[MAX_SIGN];

static int			g_TexNo;			// �e�N�X�`���ԍ�
static bool			g_bAlpaTest;		// �A���t�@�e�X�gON/OFF

static char *g_TextureName[] =
{
	"data/TEXTURE/finger.png",
};

// ���`���

static INTERPOLATION_MOVE move_tbl[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(g_cookiesign->pos.x, (g_cookiesign->pos.y + 60.0f), g_cookiesign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(g_cookiesign->pos.x, (g_cookiesign->pos.y + 40.0f), g_cookiesign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(g_cookiesign->pos.x, (g_cookiesign->pos.y + 60.0f), g_cookiesign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_MOVE move_tbl2[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(g_hotdogsign->pos.x, (g_hotdogsign->pos.y + 30.0f), g_hotdogsign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(g_hotdogsign->pos.x, (g_hotdogsign->pos.y + 10.0f), g_hotdogsign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(g_hotdogsign->pos.x, (g_hotdogsign->pos.y + 30.0f), g_hotdogsign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};

static INTERPOLATION_MOVE move_tbl3[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(g_cherrysign->pos.x, (g_cherrysign->pos.y + 30.0f), g_cherrysign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 },
	{ XMFLOAT3(g_cherrysign->pos.x, (g_cherrysign->pos.y + 10.0f), g_cherrysign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 },
	{ XMFLOAT3(g_cherrysign->pos.x, (g_cherrysign->pos.y + 30.0f), g_cherrysign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.3f, 0.3f, 0.3f), 60 },
};

static INTERPOLATION_MOVE move_tbl4[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(g_breadsign->pos.x, (g_breadsign->pos.y + 30.0f), g_breadsign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.5f, 0.5f, 0.5f), 60 },
	{ XMFLOAT3(g_breadsign->pos.x, (g_breadsign->pos.y + 10.0f), g_breadsign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.5f, 0.5f, 0.5f), 60 },
	{ XMFLOAT3(g_breadsign->pos.x, (g_breadsign->pos.y + 30.0f), g_breadsign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.5f, 0.5f, 0.5f), 60 },
};

static INTERPOLATION_MOVE move_tbl5[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(g_croissantsign->pos.x, (g_croissantsign->pos.y + 30.0f), g_croissantsign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(g_croissantsign->pos.x, (g_croissantsign->pos.y + 10.0f), g_croissantsign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
	{ XMFLOAT3(g_croissantsign->pos.x, (g_croissantsign->pos.y + 30.0f), g_croissantsign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },
};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitSign(void)
{
	MakeVertexSign();

	// get dot data
	DOT* cookie = GetCookies();
	DOT* hotdog = GetHotdog();
	DOT* cherry = GetCherry();
	DOT* bread = GetBread();
	DOT* croissant = GetCroissant();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}

	g_TexNo = 0;

	// ������
	for(int nCntSign = 0; nCntSign < MAX_SIGN; nCntSign++)
	{
		// cookie
		ZeroMemory(&g_cookiesign[nCntSign].material, sizeof(g_cookiesign[nCntSign].material));
		g_cookiesign[nCntSign].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_cookiesign[nCntSign].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_cookiesign[nCntSign].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_cookiesign[nCntSign].fWidth = SIGN_WIDTH;
		g_cookiesign[nCntSign].fHeight = SIGN_HEIGHT;
		g_cookiesign[nCntSign].bUse = false;

		// hotdog
		ZeroMemory(&g_hotdogsign[nCntSign].material, sizeof(g_hotdogsign[nCntSign].material));
		g_hotdogsign[nCntSign].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_hotdogsign[nCntSign].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_hotdogsign[nCntSign].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_hotdogsign[nCntSign].fWidth = SIGN_WIDTH;
		g_hotdogsign[nCntSign].fHeight = SIGN_HEIGHT;
		g_hotdogsign[nCntSign].bUse = false;

		// cherry
		ZeroMemory(&g_cherrysign[nCntSign].material, sizeof(g_cherrysign[nCntSign].material));
		g_cherrysign[nCntSign].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_cherrysign[nCntSign].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_cherrysign[nCntSign].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_cherrysign[nCntSign].fWidth = SIGN_WIDTH;
		g_cherrysign[nCntSign].fHeight = SIGN_HEIGHT;
		g_cherrysign[nCntSign].bUse = false;

		// bread
		ZeroMemory(&g_breadsign[nCntSign].material, sizeof(g_breadsign[nCntSign].material));
		g_breadsign[nCntSign].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_breadsign[nCntSign].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_breadsign[nCntSign].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_breadsign[nCntSign].fWidth = SIGN_WIDTH;
		g_breadsign[nCntSign].fWidth = SIGN_WIDTH;
		g_breadsign[nCntSign].fHeight = SIGN_HEIGHT;
		g_breadsign[nCntSign].bUse = false;

		// croissant
		ZeroMemory(&g_croissantsign[nCntSign].material, sizeof(g_croissantsign[nCntSign].material));
		g_croissantsign[nCntSign].material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		g_croissantsign[nCntSign].pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_croissantsign[nCntSign].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
		g_croissantsign[nCntSign].fWidth = SIGN_WIDTH;
		g_croissantsign[nCntSign].fHeight = SIGN_HEIGHT;
		g_croissantsign[nCntSign].bUse = false;


	}

	g_bAlpaTest = true;

	// sign�ݒ�
	SetSign(XMFLOAT3(cookie->pos.x, (cookie->pos.y + 40.0f), cookie->pos.z), 20.0f, 20.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetSign(XMFLOAT3(hotdog->pos.x, (hotdog->pos.y + 20.0f), hotdog->pos.z), 20.0f, 20.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetSign(XMFLOAT3(cherry->pos.x, (cherry->pos.y + 20.0f), cherry->pos.z), 20.0f, 20.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetSign(XMFLOAT3(bread->pos.x, (bread->pos.y + 20.0f), bread->pos.z), 20.0f, 20.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	SetSign(XMFLOAT3(croissant->pos.x, (croissant->pos.y + 20.0f), croissant->pos.z), 20.0f, 20.0f, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));


	// sign ���`���
	// hotdog
	move_tbl2[0] = { XMFLOAT3(g_hotdogsign->pos.x, (g_hotdogsign->pos.y + 40.0f), g_hotdogsign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 };
	move_tbl2[1] = { XMFLOAT3(g_hotdogsign->pos.x, (g_hotdogsign->pos.y + 20.0f), g_hotdogsign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 };
	move_tbl2[2] = { XMFLOAT3(g_hotdogsign->pos.x, (g_hotdogsign->pos.y + 40.0f), g_hotdogsign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 };

	// cherry
	move_tbl3[0] = { XMFLOAT3(g_cherrysign->pos.x, (g_cherrysign->pos.y + 30.0f), g_cherrysign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 };
	move_tbl3[1] = { XMFLOAT3(g_cherrysign->pos.x, (g_cherrysign->pos.y + 10.0f), g_cherrysign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 };
	move_tbl3[2] = { XMFLOAT3(g_cherrysign->pos.x, (g_cherrysign->pos.y + 30.0f), g_cherrysign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 };

	// bread
	move_tbl4[0] = { XMFLOAT3(g_breadsign->pos.x, (g_breadsign->pos.y + 30.0f), g_breadsign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 };
	move_tbl4[1] = { XMFLOAT3(g_breadsign->pos.x, (g_breadsign->pos.y + 10.0f), g_breadsign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 };
	move_tbl4[2] = { XMFLOAT3(g_breadsign->pos.x, (g_breadsign->pos.y + 30.0f), g_breadsign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 };

	// bread
	move_tbl5[0] = { XMFLOAT3(g_croissantsign->pos.x, (g_croissantsign->pos.y + 30.0f), g_croissantsign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 };
	move_tbl5[1] = { XMFLOAT3(g_croissantsign->pos.x, (g_croissantsign->pos.y + 10.0f), g_croissantsign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 };
	move_tbl5[2] = { XMFLOAT3(g_croissantsign->pos.x, (g_croissantsign->pos.y + 30.0f), g_croissantsign->pos.z), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 };


	// cookies
	g_cookiesign[0].move_time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
	g_cookiesign[0].tbl_adr = move_tbl;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_cookiesign[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_MOVE);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// hotdog
	g_hotdogsign[0].move_time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
	g_hotdogsign[0].tbl_adr = move_tbl2;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_hotdogsign[0].tbl_size = sizeof(move_tbl2) / sizeof(INTERPOLATION_MOVE);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// cherry
	g_cherrysign[0].move_time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
	g_cherrysign[0].tbl_adr = move_tbl3;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_cherrysign[0].tbl_size = sizeof(move_tbl3) / sizeof(INTERPOLATION_MOVE);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// bread
	g_breadsign[0].move_time = 0.0f;			// ���`��ԗp�̃^�C�}�[���N���A
	g_breadsign[0].tbl_adr = move_tbl4;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_breadsign[0].tbl_size = sizeof(move_tbl4) / sizeof(INTERPOLATION_MOVE);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	// croissant
	g_croissantsign[0].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_croissantsign[0].tbl_adr = move_tbl5;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_croissantsign[0].tbl_size = sizeof(move_tbl5) / sizeof(INTERPOLATION_MOVE);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitSign(void)
{
	for(int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if(g_Texture[nCntTex] != NULL)
		{// �e�N�X�`���̉��
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if(g_VertexBuffer != NULL)
	{// ���_�o�b�t�@�̉��
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateSign(void)
{
	DOT* cookies = GetCookies();
	DOT* hotdog = GetHotdog();
	DOT* cherry = GetCherry();
	DOT* bread = GetBread();
	DOT* croissant = GetCroissant();

	// �e�ʒu���L��
	for (int f = 0; f < 3; f++)
	{
		move_tbl[f].pos.x = cookies->pos.x;
		move_tbl2[f].pos.x = hotdog->pos.x;
		move_tbl3[f].pos.x = cherry->pos.x;
		move_tbl4[f].pos.x = bread->pos.x;
		move_tbl5[f].pos.x = croissant->pos.x;

		move_tbl[f].pos.z = cookies->pos.z;
		move_tbl2[f].pos.z = hotdog->pos.z;
		move_tbl3[f].pos.z = cherry->pos.z;
		move_tbl4[f].pos.z = bread->pos.z;
		move_tbl5[f].pos.z = croissant->pos.z;
	}
	

	for(int nCntSign = 0; nCntSign < MAX_SIGN; nCntSign++)
	{
		if(g_cookiesign[nCntSign].bUse)
		{
			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_cookiesign[nCntSign].nIdxShadow, XMFLOAT3(g_cookiesign[nCntSign].pos.x, 0.1f, g_cookiesign[nCntSign].pos.z));

			if (cookies->use && g_cookiesign->tbl_adr != NULL)	// ���`��Ԃ����s����H
			{									// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_cookiesign->move_time;
				float	time = g_cookiesign->move_time - index;
				int		size = g_cookiesign->tbl_size;

				float dt = 1.0f / g_cookiesign->tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_cookiesign->move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))			// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_cookiesign->move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_cookiesign->tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_cookiesign->tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_cookiesign->pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_cookiesign->tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_cookiesign->tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_cookiesign->rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_cookiesign->tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_cookiesign->tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_cookiesign->scl, s0 + scl * time);
			}
		}
		if (hotdog->use && g_hotdogsign[nCntSign].bUse)
		{
			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_hotdogsign[nCntSign].nIdxShadow, XMFLOAT3(g_hotdogsign[nCntSign].pos.x, 0.1f, g_hotdogsign[nCntSign].pos.z));

			if (g_hotdogsign->tbl_adr != NULL)	// ���`��Ԃ����s����H
			{									// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_hotdogsign->move_time;
				float	time = g_hotdogsign->move_time - index;
				int		size = g_hotdogsign->tbl_size;

				float dt = 1.0f / g_hotdogsign->tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_hotdogsign->move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_hotdogsign->move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_hotdogsign->tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_hotdogsign->tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_hotdogsign->pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_hotdogsign->tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_hotdogsign->tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_hotdogsign->rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_hotdogsign->tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_hotdogsign->tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_hotdogsign->scl, s0 + scl * time);
			}
		}
		if (cherry->use && g_cherrysign[nCntSign].bUse)
		{
			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_cherrysign[nCntSign].nIdxShadow, XMFLOAT3(g_cherrysign[nCntSign].pos.x, 0.1f, g_cherrysign[nCntSign].pos.z));

			if (g_cherrysign->tbl_adr != NULL)	// ���`��Ԃ����s����H
			{									// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_cherrysign->move_time;
				float	time = g_cherrysign->move_time - index;
				int		size = g_cherrysign->tbl_size;

				float dt = 1.0f / g_cherrysign->tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_cherrysign->move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_cherrysign->move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_cherrysign->tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_cherrysign->tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_cherrysign->pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_cherrysign->tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_cherrysign->tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_cherrysign->rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_cherrysign->tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_cherrysign->tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_cherrysign->scl, s0 + scl * time);
			}
		}
		if (bread->use && g_breadsign[nCntSign].bUse)
		{
			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_breadsign[nCntSign].nIdxShadow, XMFLOAT3(g_breadsign[nCntSign].pos.x, 0.1f, g_breadsign[nCntSign].pos.z));

			if (g_breadsign->tbl_adr != NULL)	// ���`��Ԃ����s����H
			{									// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_breadsign->move_time;
				float	time = g_breadsign->move_time - index;
				int		size = g_breadsign->tbl_size;

				float dt = 1.0f / g_breadsign->tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_breadsign->move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_breadsign->move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_breadsign->tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_breadsign->tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_breadsign->pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_breadsign->tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_breadsign->tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_breadsign->rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_breadsign->tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_breadsign->tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_breadsign->scl, s0 + scl * time);
			}
		}
		if (croissant->use && g_croissantsign[nCntSign].bUse)
		{
			// �e�̈ʒu�ݒ�
			SetPositionShadow(g_croissantsign[nCntSign].nIdxShadow, XMFLOAT3(g_croissantsign[nCntSign].pos.x, 0.1f, g_croissantsign[nCntSign].pos.z));

			if (g_croissantsign->tbl_adr != NULL)	// ���`��Ԃ����s����H
			{									// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_croissantsign->move_time;
				float	time = g_croissantsign->move_time - index;
				int		size = g_croissantsign->tbl_size;

				float dt = 1.0f / g_croissantsign->tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_croissantsign->move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_croissantsign->move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_croissantsign->tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_croissantsign->tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_croissantsign->pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_croissantsign->tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_croissantsign->tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_croissantsign->rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_croissantsign->tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_croissantsign->tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_croissantsign->scl, s0 + scl * time);
			}

		}

	}


#ifdef _DEBUG
	// �A���t�@�e�X�gON/OFF
	if(GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? false: true;
	}

#endif

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawSign(void)
{
	DOT* cookies = GetCookies();
	DOT* hotdog = GetHotdog();
	DOT* cherry = GetCherry();
	DOT* bread = GetBread();
	DOT* croissant = GetCroissant();

	// ���e�X�g�ݒ�
	if (g_bAlpaTest == true)
	{
		// ���e�X�g��L����
		SetAlphaTestEnable(true);
	}

	// ���C�e�B���O�𖳌�
	SetLightEnable(false);

	XMMATRIX mtxScl, mtxTranslate, mtxWorld, mtxView;
	CAMERA *cam = GetCamera();

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for(int i = 0; i < MAX_SIGN; i++)
	{
		if(cookies->use && g_cookiesign[i].bUse)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
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

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_cookiesign[i].scl.x, g_cookiesign[i].scl.y, g_cookiesign[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_cookiesign[i].pos.x, g_cookiesign[i].pos.y, g_cookiesign[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_cookiesign[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}

		if (hotdog->use && g_hotdogsign[i].bUse)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
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

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_hotdogsign[i].scl.x, g_hotdogsign[i].scl.y, g_hotdogsign[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_hotdogsign[i].pos.x, g_hotdogsign[i].pos.y, g_hotdogsign[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_hotdogsign[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}

		if (cherry->use && g_cherrysign[i].bUse)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
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

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_cherrysign[i].scl.x, g_cherrysign[i].scl.y, g_cherrysign[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_cherrysign[i].pos.x, g_cherrysign[i].pos.y, g_cherrysign[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_cherrysign[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}

		if (bread->use && g_breadsign[i].bUse)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
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

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_breadsign[i].scl.x, g_breadsign[i].scl.y, g_breadsign[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_breadsign[i].pos.x, g_breadsign[i].pos.y, g_breadsign[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_breadsign[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}

		if (croissant->use && g_croissantsign[i].bUse)
		{
			// ���[���h�}�g���b�N�X�̏�����
			mtxWorld = XMMatrixIdentity();

			// �r���[�}�g���b�N�X���擾
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

			// �X�P�[���𔽉f
			mtxScl = XMMatrixScaling(g_croissantsign[i].scl.x, g_croissantsign[i].scl.y, g_croissantsign[i].scl.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

			// �ړ��𔽉f
			mtxTranslate = XMMatrixTranslation(g_croissantsign[i].pos.x, g_croissantsign[i].pos.y, g_croissantsign[i].pos.z);
			mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			SetWorldMatrix(&mtxWorld);


			// �}�e���A���ݒ�
			SetMaterial(g_croissantsign[i].material);

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

			// �|���S���̕`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(true);

	// ���e�X�g�𖳌���
	SetAlphaTestEnable(false);
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexSign(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// ���_�o�b�t�@�ɒl���Z�b�g����
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = 30.0f;
	float fHeight = 30.0f;

	// ���_���W�̐ݒ�
	vertex[0].Position = XMFLOAT3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = XMFLOAT3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = XMFLOAT3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = XMFLOAT3(fWidth / 2.0f, 0.0f, 0.0f);

	// �g�U���̐ݒ�
	vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);
	vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);
	vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);
	vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// �T�C���̃p�����[�^���Z�b�g
//=============================================================================
int SetSign(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col)
{
	int nIdxSign = -1;

	for(int nCntSign = 0; nCntSign < MAX_SIGN; nCntSign++)
	{
		if(!g_cookiesign[nCntSign].bUse)
		{
			g_cookiesign[nCntSign].pos = pos;
			g_cookiesign[nCntSign].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_cookiesign[nCntSign].fWidth = fWidth;
			g_cookiesign[nCntSign].fHeight = fHeight;
			g_cookiesign[nCntSign].bUse = true;

			// �e�̐ݒ�
			g_cookiesign[nCntSign].nIdxShadow = CreateShadow(g_cookiesign[nCntSign].pos, 0.5f, 0.5f);

			nIdxSign = nCntSign;

			break;
		}
		if (!g_hotdogsign[nCntSign].bUse)
		{
			g_hotdogsign[nCntSign].pos = pos;
			g_hotdogsign[nCntSign].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_hotdogsign[nCntSign].fWidth = fWidth;
			g_hotdogsign[nCntSign].fHeight = fHeight;
			g_hotdogsign[nCntSign].bUse = true;

			// �e�̐ݒ�
			g_hotdogsign[nCntSign].nIdxShadow = CreateShadow(g_hotdogsign[nCntSign].pos, 0.5f, 0.5f);

			nIdxSign = nCntSign;

			break;
		}
		if (!g_cherrysign[nCntSign].bUse)
		{
			g_cherrysign[nCntSign].pos = pos;
			g_cherrysign[nCntSign].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_cherrysign[nCntSign].fWidth = fWidth;
			g_cherrysign[nCntSign].fHeight = fHeight;
			g_cherrysign[nCntSign].bUse = true;

			// �e�̐ݒ�
			g_cherrysign[nCntSign].nIdxShadow = CreateShadow(g_cherrysign[nCntSign].pos, 0.5f, 0.5f);

			nIdxSign = nCntSign;

			break;
		}
		if (!g_breadsign[nCntSign].bUse)
		{
			g_breadsign[nCntSign].pos = pos;
			g_breadsign[nCntSign].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_breadsign[nCntSign].fWidth = fWidth;
			g_breadsign[nCntSign].fHeight = fHeight;
			g_breadsign[nCntSign].bUse = true;

			// �e�̐ݒ�
			g_breadsign[nCntSign].nIdxShadow = CreateShadow(g_breadsign[nCntSign].pos, 0.5f, 0.5f);

			nIdxSign = nCntSign;

			break;
		}
		if (!g_croissantsign[nCntSign].bUse)
		{
			g_croissantsign[nCntSign].pos = pos;
			g_croissantsign[nCntSign].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);
			g_croissantsign[nCntSign].fWidth = fWidth;
			g_croissantsign[nCntSign].fHeight = fHeight;
			g_croissantsign[nCntSign].bUse = true;

			// �e�̐ݒ�
			g_croissantsign[nCntSign].nIdxShadow = CreateShadow(g_croissantsign[nCntSign].pos, 0.5f, 0.5f);

			nIdxSign = nCntSign;

			break;
		}
	}

	return nIdxSign;
}