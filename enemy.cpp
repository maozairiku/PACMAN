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

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_ENEMY_GR		"data/MODEL/ghostred.obj"		// Ghost_Red
#define	MODEL_ENEMY_GO		"data/MODEL/ghostorange.obj"	// Ghost_Orange
#define	MODEL_ENEMY_GG		"data/MODEL/ghostgreen.obj"		// Ghost_Green
#define	MODEL_ENEMY_GB		"data/MODEL/ghostblue.obj"		// Ghost_Blue
#define	MODEL_ENEMY_GP		"data/MODEL/ghostpurple.obj"	// Ghost_Purple
#define	MODEL_ENEMY_GY		"data/MODEL/ghostgray.obj"		// Ghost_Gray

#define	VALUE_MOVE			(5.0f)						// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)				// ��]��

#define ENEMY_SHADOW_SIZE	(0.4f)						// �e�̑傫��
#define ENEMY_OFFSET_Y		(7.0f)						// �G�l�~�[�̑��������킹��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ENEMY			g_GhostRed[MAX_ENEMY];				// �G�l�~�[
static ENEMY			g_GhostOrange[MAX_ENEMY];
static ENEMY			g_GhostGreen[MAX_ENEMY];
static ENEMY			g_GhostBlue[MAX_ENEMY];
static ENEMY			g_GhostPurple[MAX_ENEMY];
static ENEMY			g_GhostGray[MAX_ENEMY];

static BOOL				g_Load = FALSE;


static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(   0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*2 },
	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*1 },
	{ XMFLOAT3(-200.0f, ENEMY_OFFSET_Y, 200.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*0.5f },
	{ XMFLOAT3(   0.0f, ENEMY_OFFSET_Y,  20.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60*2 },

};


//=============================================================================
// ����������
//=============================================================================
HRESULT InitEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		// Ghost_Red
		LoadModel(MODEL_ENEMY_GR, &g_GhostRed[i].model);
		g_GhostRed[i].load = true;

		g_GhostRed[i].pos = XMFLOAT3(310.0f, ENEMY_OFFSET_Y, 435.0f);
		g_GhostRed[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_GhostRed[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_GhostRed[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_GhostRed[i].size = ENEMY_SIZE;	// �����蔻��̑傫��
		g_GhostRed[i].colorIdx = 1;

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_GhostRed[0].model, &g_GhostRed[0].diffuse[0]);

		XMFLOAT3 rpos = g_GhostRed[i].pos;
		rpos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_GhostRed[i].shadowIdx = CreateShadow(rpos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_GhostRed[i].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		g_GhostRed[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_GhostRed[i].tbl_size = 0;			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_GhostRed[i].use = true;			// true:�����Ă�


		// Ghost_Orange
		LoadModel(MODEL_ENEMY_GO, &g_GhostOrange[i].model);
		g_GhostOrange[i].load = true;

		g_GhostOrange[i].pos = XMFLOAT3(30.0f, ENEMY_OFFSET_Y, 20.0f);
		g_GhostOrange[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_GhostOrange[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_GhostOrange[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_GhostOrange[i].size = ENEMY_SIZE;		// �����蔻��̑傫��
		g_GhostOrange[i].colorIdx = 2;

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_GhostOrange[0].model, &g_GhostOrange[0].diffuse[0]);

		XMFLOAT3 opos = g_GhostOrange[i].pos;
		opos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_GhostOrange[i].shadowIdx = CreateShadow(opos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_GhostOrange[i].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		g_GhostOrange[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_GhostOrange[i].tbl_size = 0;			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_GhostOrange[i].use = true;			// true:�����Ă�


		// Ghost_Green
		LoadModel(MODEL_ENEMY_GO, &g_GhostGreen[i].model);
		g_GhostGreen[i].load = true;

		g_GhostGreen[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);
		g_GhostGreen[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_GhostGreen[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_GhostGreen[i].spd = 0.0f;				// �ړ��X�s�[�h�N���A
		g_GhostGreen[i].size = ENEMY_SIZE;		// �����蔻��̑傫��
		g_GhostGreen[i].colorIdx = 3;

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_GhostGreen[0].model, &g_GhostGreen[0].diffuse[0]);

		XMFLOAT3 gpos = g_GhostOrange[i].pos;
		gpos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_GhostGreen[i].shadowIdx = CreateShadow(gpos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_GhostGreen[i].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		g_GhostGreen[i].tbl_adr = NULL;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_GhostGreen[i].tbl_size = 0;			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_GhostGreen[i].use = true;				// true:�����Ă�


		// Ghost_Blue
		LoadModel(MODEL_ENEMY_GO, &g_GhostBlue[i].model);
		g_GhostBlue[i].load = true;

		g_GhostBlue[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);
		g_GhostBlue[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_GhostBlue[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_GhostBlue[i].spd = 0.0f;				// �ړ��X�s�[�h�N���A
		g_GhostBlue[i].size = ENEMY_SIZE;		// �����蔻��̑傫��
		g_GhostBlue[i].colorIdx = 4;

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_GhostBlue[0].model, &g_GhostBlue[0].diffuse[0]);

		XMFLOAT3 bpos = g_GhostBlue[i].pos;
		bpos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_GhostBlue[i].shadowIdx = CreateShadow(bpos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_GhostBlue[i].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		g_GhostBlue[i].tbl_adr = NULL;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_GhostBlue[i].tbl_size = 0;			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_GhostBlue[i].use = true;				// true:�����Ă�


		// Ghost_Purple
		LoadModel(MODEL_ENEMY_GR, &g_GhostPurple[i].model);
		g_GhostPurple[i].load = true;
		g_GhostPurple[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);
		g_GhostPurple[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_GhostPurple[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_GhostPurple[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_GhostPurple[i].size = ENEMY_SIZE;		// �����蔻��̑傫��
		g_GhostPurple[i].colorIdx = 5;

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_GhostPurple[0].model, &g_GhostPurple[0].diffuse[0]);

		XMFLOAT3 ppos = g_GhostPurple[i].pos;
		ppos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_GhostPurple[i].shadowIdx = CreateShadow(ppos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_GhostPurple[i].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		g_GhostPurple[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_GhostPurple[i].tbl_size = 0;			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_GhostPurple[i].use = true;			// true:�����Ă�


		// Ghost_Gray
		LoadModel(MODEL_ENEMY_GR, &g_GhostGray[i].model);
		g_GhostGray[i].load = true;
		g_GhostGray[i].pos = XMFLOAT3(-50.0f + i * 30.0f, ENEMY_OFFSET_Y, 20.0f);
		g_GhostGray[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_GhostGray[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_GhostGray[i].spd = 0.0f;				// �ړ��X�s�[�h�N���A
		g_GhostGray[i].size = ENEMY_SIZE;		// �����蔻��̑傫��
		g_GhostGray[i].colorIdx = 6;

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_GhostPurple[0].model, &g_GhostPurple[0].diffuse[0]);

		XMFLOAT3 ypos = g_GhostPurple[i].pos;
		ypos.y -= (ENEMY_OFFSET_Y - 0.1f);
		g_GhostGray[i].shadowIdx = CreateShadow(ypos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

		g_GhostGray[i].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		g_GhostGray[i].tbl_adr = NULL;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_GhostGray[i].tbl_size = 0;			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_GhostGray[i].use = true;				// true:�����Ă�

	}


	// 0�Ԃ������`��Ԃœ������Ă݂�
	g_GhostRed[0].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
	g_GhostRed[0].tbl_adr = move_tbl;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
	g_GhostRed[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

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
		if (g_GhostOrange[i].load)
		{
			UnloadModel(&g_GhostOrange[i].model);
			g_GhostOrange[i].load = false;
		}
		if (g_GhostGreen[i].load)
		{
			UnloadModel(&g_GhostGreen[i].model);
			g_GhostGreen[i].load = false;
		}
		if (g_GhostBlue[i].load)
		{
			UnloadModel(&g_GhostBlue[i].model);
			g_GhostBlue[i].load = false;
		}
		if (g_GhostPurple[i].load)
		{
			UnloadModel(&g_GhostPurple[i].model);
			g_GhostPurple[i].load = false;
		}
		if (g_GhostGray[i].load)
		{
			UnloadModel(&g_GhostGray[i].model);
			g_GhostGray[i].load = false;
		}

	}

	g_Load = FALSE;

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateEnemy(void)
{
	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_ENEMY; i++)
	{
		// Ghost_Red
		if (g_GhostRed[i].use == true)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			if (g_GhostRed[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
			{								// ���`��Ԃ̏���
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

			// ���C�L���X�g���đ����̍��������߂�
			XMFLOAT3 HitPosition;		// ��_
			XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
			bool ans = RayHitField(g_GhostRed[i].pos, &HitPosition, &Normal);
			if (ans)
			{
				g_GhostRed[i].pos.y = HitPosition.y + ENEMY_OFFSET_Y;
			}
			else
			{
				g_GhostRed[i].pos.y = ENEMY_OFFSET_Y;
				Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			}

			// �e���G�l�~�[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_GhostRed[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_GhostRed[i].shadowIdx, pos);
		}

		// Ghost_Orange
		if (g_GhostOrange[i].use == true)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			if (g_GhostOrange[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
			{								// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_GhostOrange[i].move_time;
				float	time = g_GhostOrange[i].move_time - index;
				int		size = g_GhostOrange[i].tbl_size;

				float dt = 1.0f / g_GhostOrange[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_GhostOrange[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_GhostOrange[i].move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_GhostOrange[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_GhostOrange[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_GhostRed[i].pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_GhostOrange[i].tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_GhostOrange[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_GhostRed[i].rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_GhostOrange[i].tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_GhostOrange[i].tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_GhostOrange[i].scl, s0 + scl * time);

			}

			// ���C�L���X�g���đ����̍��������߂�
			XMFLOAT3 HitPosition;		// ��_
			XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
			bool ans = RayHitField(g_GhostOrange[i].pos, &HitPosition, &Normal);
			if (ans)
			{
				g_GhostOrange[i].pos.y = HitPosition.y + ENEMY_OFFSET_Y;
			}
			else
			{
				g_GhostOrange[i].pos.y = ENEMY_OFFSET_Y;
				Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			}

			// �e���G�l�~�[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_GhostOrange[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_GhostOrange[i].shadowIdx, pos);
		}

		// Ghost_Green
		if (g_GhostGreen[i].use == true)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			if (g_GhostGreen[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
			{								// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_GhostGreen[i].move_time;
				float	time = g_GhostGreen[i].move_time - index;
				int		size = g_GhostGreen[i].tbl_size;

				float dt = 1.0f / g_GhostGreen[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_GhostGreen[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_GhostGreen[i].move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_GhostGreen[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_GhostGreen[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_GhostRed[i].pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_GhostGreen[i].tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_GhostGreen[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_GhostRed[i].rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_GhostGreen[i].tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_GhostGreen[i].tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_GhostGreen[i].scl, s0 + scl * time);

			}

			// ���C�L���X�g���đ����̍��������߂�
			XMFLOAT3 HitPosition;		// ��_
			XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
			bool ans = RayHitField(g_GhostGreen[i].pos, &HitPosition, &Normal);
			if (ans)
			{
				g_GhostGreen[i].pos.y = HitPosition.y + ENEMY_OFFSET_Y;
			}
			else
			{
				g_GhostGreen[i].pos.y = ENEMY_OFFSET_Y;
				Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			}

			// �e���G�l�~�[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_GhostGreen[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_GhostGreen[i].shadowIdx, pos);
		}

		// Ghost_Blue
		if (g_GhostBlue[i].use == true)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			if (g_GhostBlue[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
			{								// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_GhostBlue[i].move_time;
				float	time = g_GhostBlue[i].move_time - index;
				int		size = g_GhostBlue[i].tbl_size;

				float dt = 1.0f / g_GhostBlue[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_GhostBlue[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_GhostBlue[i].move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_GhostBlue[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_GhostBlue[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_GhostRed[i].pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_GhostBlue[i].tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_GhostBlue[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_GhostRed[i].rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_GhostBlue[i].tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_GhostBlue[i].tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_GhostBlue[i].scl, s0 + scl * time);

			}

			// ���C�L���X�g���đ����̍��������߂�
			XMFLOAT3 HitPosition;		// ��_
			XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
			bool ans = RayHitField(g_GhostBlue[i].pos, &HitPosition, &Normal);
			if (ans)
			{
				g_GhostBlue[i].pos.y = HitPosition.y + ENEMY_OFFSET_Y;
			}
			else
			{
				g_GhostBlue[i].pos.y = ENEMY_OFFSET_Y;
				Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			}

			// �e���G�l�~�[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_GhostBlue[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_GhostBlue[i].shadowIdx, pos);
		}

		// Ghost_Purple
		if (g_GhostPurple[i].use == true)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			if (g_GhostPurple[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
			{								// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_GhostPurple[i].move_time;
				float	time = g_GhostPurple[i].move_time - index;
				int		size = g_GhostPurple[i].tbl_size;

				float dt = 1.0f / g_GhostPurple[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_GhostPurple[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_GhostPurple[i].move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_GhostPurple[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_GhostPurple[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_GhostRed[i].pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_GhostPurple[i].tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_GhostPurple[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_GhostRed[i].rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_GhostPurple[i].tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_GhostPurple[i].tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_GhostPurple[i].scl, s0 + scl * time);

			}

			// ���C�L���X�g���đ����̍��������߂�
			XMFLOAT3 HitPosition;		// ��_
			XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
			bool ans = RayHitField(g_GhostPurple[i].pos, &HitPosition, &Normal);
			if (ans)
			{
				g_GhostPurple[i].pos.y = HitPosition.y + ENEMY_OFFSET_Y;
			}
			else
			{
				g_GhostPurple[i].pos.y = ENEMY_OFFSET_Y;
				Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			}

			// �e���G�l�~�[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_GhostPurple[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_GhostPurple[i].shadowIdx, pos);
		}

		// Ghost_Gray
		if (g_GhostGray[i].use == true)			// ���̃G�l�~�[���g���Ă���H
		{									// Yes
			if (g_GhostPurple[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
			{								// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_GhostGray[i].move_time;
				float	time = g_GhostGray[i].move_time - index;
				int		size = g_GhostGray[i].tbl_size;

				float dt = 1.0f / g_GhostGray[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_GhostGray[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_GhostGray[i].move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_GhostGray[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_GhostGray[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_GhostRed[i].pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_GhostGray[i].tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_GhostGray[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_GhostRed[i].rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_GhostGray[i].tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_GhostGray[i].tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_GhostGray[i].scl, s0 + scl * time);

			}

			// ���C�L���X�g���đ����̍��������߂�
			XMFLOAT3 HitPosition;		// ��_
			XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
			bool ans = RayHitField(g_GhostGray[i].pos, &HitPosition, &Normal);
			if (ans)
			{
				g_GhostGray[i].pos.y = HitPosition.y + ENEMY_OFFSET_Y;
			}
			else
			{
				g_GhostGray[i].pos.y = ENEMY_OFFSET_Y;
				Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			}

			// �e���G�l�~�[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_GhostGray[i].pos;
			pos.y -= (ENEMY_OFFSET_Y - 0.1f);
			SetPositionShadow(g_GhostGray[i].shadowIdx, pos);
		}

	}

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
		DrawModel(&g_GhostRed[i].model);


		// Ghost Orange
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
		DrawModel(&g_GhostOrange[i].model);


		// Ghost Green
		if (g_GhostGreen[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_GhostGreen[i].scl.x, g_GhostGreen[i].scl.y, g_GhostGreen[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_GhostGreen[i].rot.x, g_GhostGreen[i].rot.y + XM_PI, g_GhostGreen[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_GhostGreen[i].pos.x, g_GhostGreen[i].pos.y, g_GhostGreen[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_GhostGreen[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_GhostGreen[i].model);


		// Ghost Blue
		if (g_GhostBlue[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_GhostBlue[i].scl.x, g_GhostBlue[i].scl.y, g_GhostBlue[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_GhostBlue[i].rot.x, g_GhostBlue[i].rot.y + XM_PI, g_GhostBlue[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_GhostBlue[i].pos.x, g_GhostBlue[i].pos.y, g_GhostBlue[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_GhostBlue[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_GhostBlue[i].model);


		// Ghost Purple
		if (g_GhostPurple[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_GhostPurple[i].scl.x, g_GhostPurple[i].scl.y, g_GhostPurple[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_GhostPurple[i].rot.x, g_GhostPurple[i].rot.y + XM_PI, g_GhostPurple[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_GhostPurple[i].pos.x, g_GhostPurple[i].pos.y, g_GhostPurple[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_GhostPurple[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_GhostPurple[i].model);


		// Ghost Gray
		if (g_GhostGray[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_GhostGray[i].scl.x, g_GhostGray[i].scl.y, g_GhostGray[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_GhostGray[i].rot.x, g_GhostGray[i].rot.y + XM_PI, g_GhostGray[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_GhostGray[i].pos.x, g_GhostGray[i].pos.y, g_GhostGray[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_GhostGray[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_GhostGray[i].model);

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
ENEMY* GetGhostGreen()
{
	return &g_GhostGreen[0];
}
ENEMY* GetGhostBlue()
{
	return &g_GhostBlue[0];
}
ENEMY* GetGhostPurple()
{
	return &g_GhostPurple[0];
}
ENEMY* GetGhostGray()
{
	return &g_GhostGray[0];
}

