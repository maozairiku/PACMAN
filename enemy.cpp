//=============================================================================
//
// �G�l�~�[���f������ [enemy.cpp]
// Author : ���@�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "enemy.h"
#include "shadow.h"
#include "meshfield.h"
#include "bullet.h"
#include "particle.h"
#include "player.h"
#include <time.h>

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY_GR		"data/MODEL/ghostred.obj"		// Ghost_Red
#define	MODEL_ENEMY_GO		"data/MODEL/orange.obj"			// Ghost_Orange
#define	MODEL_ENEMY_GG		"data/MODEL/ghostgreen.obj"		// Ghost_Green
#define	MODEL_ENEMY_GB		"data/MODEL/ghostblue.obj"		// Ghost_Blue
#define	MODEL_ENEMY_GP		"data/MODEL/ghostpurple.obj"	// Ghost_Purple

#define	VALUE_MOVE			(5.0f)							// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// ��]��

#define ENEMY_SHADOW_SIZE	(0.4f)							// �e�̑傫��
#define ENEMY_OFFSET_Y		(20.0f)							// �G�l�~�[�̑��������킹��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_GhostRed[MAX_ENEMY];				// �G�l�~�[
static ENEMY			g_GhostOrange[ORANGE_MAX];

static float g_angle = 0;

static BOOL				g_Load = FALSE;

static float orangeRadius[ORANGE_MAX];

static float g_timer = -1;


// red_ghost�B�̐��`���
static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(576.0f, ENEMY_OFFSET_Y, 565.0f), XMFLOAT3(0.0f, XMConvertToRadians(180), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1},
	{ XMFLOAT3(576.0f, ENEMY_OFFSET_Y, 565.0f), XMFLOAT3(0.0f, XMConvertToRadians(0), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1},
	{ XMFLOAT3(576.0f, ENEMY_OFFSET_Y, 565.0f), XMFLOAT3(0.0f, XMConvertToRadians(-180), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1},
};

static INTERPOLATION_DATA move_tbl2[] = {
	{ XMFLOAT3(580.0f, ENEMY_OFFSET_Y, -580.0f), XMFLOAT3(0.0f, XMConvertToRadians(180), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 2},
	{ XMFLOAT3(580.0f, ENEMY_OFFSET_Y, -580.0f), XMFLOAT3(0.0f, XMConvertToRadians(0), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 2},
	{ XMFLOAT3(580.0f, ENEMY_OFFSET_Y, -580.0f), XMFLOAT3(0.0f, XMConvertToRadians(-180), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 2},
};

static INTERPOLATION_DATA move_tbl3[] = {
	{ XMFLOAT3(-490.0f, ENEMY_OFFSET_Y, -475.0f), XMFLOAT3(0.0f, XMConvertToRadians(180), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1.5},
	{ XMFLOAT3(-490.0f, ENEMY_OFFSET_Y, -475.0f), XMFLOAT3(0.0f, XMConvertToRadians(0), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1.5},
	{ XMFLOAT3(-490.0f, ENEMY_OFFSET_Y, -475.0f), XMFLOAT3(0.0f, XMConvertToRadians(-180), 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 * 1.5},
};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	srand((unsigned int)time(NULL));
	g_timer = -1;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		// Ghost_Red
		LoadModel(MODEL_ENEMY_GR, &g_GhostRed[i].model);
		g_GhostRed[i].load = true;

		g_GhostRed[0].pos = XMFLOAT3(576.0f, ENEMY_OFFSET_Y, 565.0f);
		g_GhostRed[1].pos = XMFLOAT3(580.0f, ENEMY_OFFSET_Y, -580.0f);
		g_GhostRed[2].pos = XMFLOAT3(-490.0f, ENEMY_OFFSET_Y, -475.0f);

		g_GhostRed[i].rot = XMFLOAT3(0.0f, 360.0f, 0.0f);
		g_GhostRed[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_GhostRed[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_GhostRed[i].size = ENEMY_SIZE;	// �����蔻��̑傫��
		g_GhostRed[i].colorIdx = 1;
		g_GhostRed[i].attackflame = 0;

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_GhostRed[0].model, &g_GhostRed[0].diffuse[0]);

		XMFLOAT3 rpos = g_GhostRed[i].pos;
		rpos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_GhostRed[i].shadowIdx = CreateShadow(rpos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_GhostRed[i].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		g_GhostRed[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_GhostRed[i].tbl_size = 0;			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_GhostRed[i].use = true;			// true:�����Ă�

	}

	// orange ghost
	LoadModel(MODEL_ENEMY_GO, &g_GhostOrange[0].model);
	g_GhostOrange[0].load = true;

	for (int i = 0; i < ORANGE_MAX; i++)
	{
		g_GhostOrange[i].pos = XMFLOAT3(rand()%1300 - 650, ENEMY_OFFSET_Y, rand()%1300 - 650);
		g_GhostOrange[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_GhostOrange[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_GhostOrange[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_GhostOrange[i].size = ENEMY_SIZE;		// �����蔻��̑傫��
		g_GhostOrange[i].colorIdx = 2;
		g_GhostOrange[i].attackflame = 0;

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_GhostOrange[0].model, &g_GhostOrange[0].diffuse[0]);

		XMFLOAT3 opos = g_GhostOrange[i].pos;
		opos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_GhostOrange[i].shadowIdx = CreateShadow(opos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_GhostOrange[i].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		g_GhostOrange[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_GhostOrange[i].tbl_size = 0;			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_GhostOrange[i].use = true;			// true:�����Ă�

		orangeRadius[i] = rand() % 10;
	}

	// ghost red�B�͐��`��Ԃœ������Ă݂�
	g_GhostRed[0].move_time = 0.0f;				// ���`��ԗp�̃^�C�}�[���N���A
	g_GhostRed[0].tbl_adr = move_tbl;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_GhostRed[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	g_GhostRed[1].move_time = 0.0f;				// ���`��ԗp�̃^�C�}�[���N���A
	g_GhostRed[1].tbl_adr = move_tbl2;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_GhostRed[1].tbl_size = sizeof(move_tbl2) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	g_GhostRed[2].move_time = 0.0f;				// ���`��ԗp�̃^�C�}�[���N���A
	g_GhostRed[2].tbl_adr = move_tbl3;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_GhostRed[2].tbl_size = sizeof(move_tbl3) / sizeof(INTERPOLATION_DATA);		// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitEnemy(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (g_GhostRed[i].load)
		{
			UnloadModel(&g_GhostRed[i].model);
			g_GhostRed[i].load = false;
		}
	}

	if (g_GhostOrange[0].load)
	{
		UnloadModel(&g_GhostOrange[0].model);
		g_GhostOrange[0].load = false;
	}

	g_Load = FALSE;

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	// move time
	g_timer++;

	// for particle
	int nLife = 150;
	float fSize = 50.0f;

	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		BULLET* bullet = GetBullet();
		PLAYER* player = GetPlayer();

		// Ghost_Red
		if (g_GhostRed[i].use == true)			// ���̃G�l�~�[���g���Ă���H
		{										// Yes
			if (g_GhostRed[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
			{									// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_GhostRed[i].move_time;
				float	time = g_GhostRed[i].move_time - index;
				int		size = g_GhostRed[i].tbl_size;

				float dt = 1.0f / g_GhostRed[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_GhostRed[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_GhostRed[i].move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_GhostRed[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_GhostRed[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_GhostRed[i].pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_GhostRed[i].tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_GhostRed[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_GhostRed[i].rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_GhostRed[i].tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_GhostRed[i].tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_GhostRed[i].scl, s0 + scl * time);

			}

			//// ���C�L���X�g���đ����̍��������߂�
			//XMFLOAT3 HitPosition;		// ��_
			//XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
			//bool ans = RayHitField(g_GhostRed[i].pos, &HitPosition, &Normal);
			//if (ans)
			//{
			//	g_GhostRed[i].pos.y = HitPosition.y + ENEMY_OFFSET_Y;
			//}
			//else
			//{
			//	g_GhostRed[i].pos.y = ENEMY_OFFSET_Y;
			//	Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			//}

			// �e���G�l�~�[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_GhostRed[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_GhostRed[i].shadowIdx, pos);

			// enemy attack frame count
			g_GhostRed[i].attackflame++;

			if ((int)g_timer % 50 == 0)
			{
				SetBulletBezier(g_GhostRed[i].pos, g_GhostRed[i].rot, player->pos);
			}

			// particle
			//SetParticle(g_GhostRed[i].pos, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), fSize, fSize, nLife);
		}
	}

	// enemy cercle move
	float radian = XMConvertToRadians(g_angle);
	if (radian >= XM_2PI)
	{
		radian -= XM_2PI;
		g_angle = 0;
	}

	for (int i = 0; i < ORANGE_MAX; i++)
	{
		if (g_GhostOrange[i].use == true)
		{
			g_GhostOrange[i].pos.x += orangeRadius[i] * cosf(radian);
			g_GhostOrange[i].pos.z += orangeRadius[i] * sinf(radian);
			g_GhostOrange[i].rot.y += 0.1f;

			if ((int)g_timer % 60 == 0)
			{
				SetBullet(g_GhostOrange[i].pos, g_GhostOrange[i].rot);
			}

			// orange ghost move particle
			SetParticle(g_GhostOrange[i].pos, XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 0.85f), fSize, fSize, nLife);
		}
	}

	g_angle++;
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawEnemy(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		// Ghost Red
		if (g_GhostRed[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_GhostRed[i].scl.x, g_GhostRed[i].scl.y, g_GhostRed[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_GhostRed[i].rot.x, g_GhostRed[i].rot.y + XM_PI, g_GhostRed[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_GhostRed[i].pos.x, g_GhostRed[i].pos.y, g_GhostRed[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_GhostRed[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_GhostRed[0].model);
	}

	for (int i = 0; i < ORANGE_MAX; i++)
	{
		if (g_GhostOrange[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_GhostOrange[i].scl.x, g_GhostOrange[i].scl.y, g_GhostOrange[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_GhostOrange[i].rot.x, g_GhostOrange[i].rot.y + XM_PI, g_GhostOrange[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_GhostOrange[i].pos.x, g_GhostOrange[i].pos.y, g_GhostOrange[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_GhostOrange[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_GhostOrange[0].model);
	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);
}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
ENEMY *GetGhostRed()
{
	return &g_GhostRed[0];
}
ENEMY* GetGhostOrange()
{
	return &g_GhostOrange[0];
}

