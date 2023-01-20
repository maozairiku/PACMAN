//=============================================================================
//
// �G�l�~�[���f������ [dot.cpp]
// Author : ���@�E��
//
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "model.h"
#include "dot.h"
#include "shadow.h"
#include "meshfield.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_COOKIES		"data/MODEL/cookies.obj"		// model
#define	MODEL_HOTDOG		"data/MODEL/hotdog.obj"		
#define	MODEL_CHERRY		"data/MODEL/cherry.obj"	
#define	MODEL_BREAD			"data/MODEL/bread.obj"	
#define	MODEL_CROISSANT		"data/MODEL/croissant.obj"	


#define	VALUE_MOVE			(5.0f)							// �ړ���
#define	VALUE_ROTATE		(XM_PI * 0.02f)					// ��]��

#define DOT_SHADOW_SIZE		(0.4f)							// �e�̑傫��
#define DOT_OFFSET_Y		(10.0f)							// �G�l�~�[�̑��������킹��


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static DOT			g_Cookies[MAX_DOT];			// �h�b�g��
static DOT			g_Hotdog[MAX_DOT];			
static DOT			g_Cherry[MAX_DOT];		
static DOT			g_Bread[MAX_DOT];		
static DOT			g_Croissant[MAX_DOT];			


static BOOL			g_Load = FALSE;


static INTERPOLATION_DATA move_tbl[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(0.0f, DOT_OFFSET_Y, 0.0f), XMFLOAT3(0.0f, 360.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl2[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-300.0f, DOT_OFFSET_Y, 280.0f), XMFLOAT3(0.0f, 360.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl3[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(-400.0f, DOT_OFFSET_Y, -333.0f), XMFLOAT3(0.0f, 360.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl4[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(200.0f, DOT_OFFSET_Y, -363.0f), XMFLOAT3(0.0f, 360.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};

static INTERPOLATION_DATA move_tbl5[] = {	// pos, rot, scl, frame
	{ XMFLOAT3(538.0f, DOT_OFFSET_Y, 434.0f), XMFLOAT3(0.0f, 360.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), 60 },

};



//=============================================================================
// ����������
//=============================================================================
HRESULT InitDot(void)
{
	for (int i = 0; i < MAX_DOT; i++)
	{
		// COOKIES
		LoadModel(MODEL_COOKIES, &g_Cookies[i].model);
		g_Cookies[i].load = true;

		g_Cookies[i].pos = XMFLOAT3(0.0f, DOT_OFFSET_Y, 0.0f);

		g_Cookies[i].rot = XMFLOAT3(0.0f, 45.0f, 0.0f);
		g_Cookies[i].scl = XMFLOAT3(1.0f, 1.0f, 1.0f);

		g_Cookies[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Cookies[i].size = DOT_SIZE;		// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Cookies[0].model, &g_Cookies[0].diffuse[0]);

		XMFLOAT3 cpos = g_Cookies[i].pos;
		cpos.y -= (DOT_OFFSET_Y - 0.1f);
		g_Cookies[i].shadowIdx = CreateShadow(cpos, DOT_SHADOW_SIZE, DOT_SHADOW_SIZE);

		g_Cookies[i].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		g_Cookies[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Cookies[i].tbl_size = 0;			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_Cookies[i].use = true;			// true:�����Ă�


		// HOTDOG
		LoadModel(MODEL_HOTDOG, &g_Hotdog[i].model);
		g_Hotdog[i].load = true;

		g_Hotdog[i].pos = XMFLOAT3(-300.0f, DOT_OFFSET_Y, 280.0f);

		g_Hotdog[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Hotdog[i].scl = XMFLOAT3(0.5f, 0.5f, 0.5f);

		g_Hotdog[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Hotdog[i].size = DOT_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Hotdog[0].model, &g_Hotdog[0].diffuse[0]);

		XMFLOAT3 hpos = g_Cookies[i].pos;
		hpos.y -= (DOT_OFFSET_Y - 0.1f);
		g_Hotdog[i].shadowIdx = CreateShadow(hpos, DOT_SHADOW_SIZE, DOT_SHADOW_SIZE);

		g_Hotdog[i].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		g_Hotdog[i].tbl_adr = NULL;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Hotdog[i].tbl_size = 0;			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_Hotdog[i].use = true;				// true:�����Ă�


		// CHERRY
		LoadModel(MODEL_CHERRY, &g_Cherry[i].model);
		g_Cherry[i].load = true;

		g_Cherry[0].pos = XMFLOAT3(-400.0f, DOT_OFFSET_Y, -333.0f);

		g_Cherry[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Cherry[i].scl = XMFLOAT3(0.5f, 0.5f, 0.5f);

		g_Cherry[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Cherry[i].size = DOT_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Cherry[0].model, &g_Cherry[0].diffuse[0]);

		XMFLOAT3 chpos = g_Cherry[i].pos;
		chpos.y -= (DOT_OFFSET_Y - 0.1f);
		g_Cherry[i].shadowIdx = CreateShadow(chpos, DOT_SHADOW_SIZE, DOT_SHADOW_SIZE);

		g_Cherry[i].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		g_Cherry[i].tbl_adr = NULL;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Cherry[i].tbl_size = 0;			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_Cherry[i].use = true;				// true:�����Ă�


		// BREAD
		LoadModel(MODEL_BREAD, &g_Bread[i].model);
		g_Bread[i].load = true;

		g_Bread[i].pos = XMFLOAT3(200.0f, DOT_OFFSET_Y, -363.0f);

		g_Bread[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Bread[i].scl = XMFLOAT3(0.5f, 0.5f, 0.5f);

		g_Bread[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Bread[i].size = DOT_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Bread[0].model, &g_Bread[0].diffuse[0]);

		XMFLOAT3 bpos = g_Bread[i].pos;
		bpos.y -= (DOT_OFFSET_Y - 0.1f);
		g_Bread[i].shadowIdx = CreateShadow(bpos, DOT_SHADOW_SIZE, DOT_SHADOW_SIZE);

		g_Bread[i].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		g_Bread[i].tbl_adr = NULL;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Bread[i].tbl_size = 0;			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_Bread[i].use = true;			// true:�����Ă�


		// CROISSANT	
		LoadModel(MODEL_CROISSANT, &g_Croissant[i].model);
		g_Croissant[i].load = true;

		g_Croissant[i].pos = XMFLOAT3(538.0f, DOT_OFFSET_Y, 434.0f);

		g_Croissant[i].rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		g_Croissant[i].scl = XMFLOAT3(0.5f, 0.5f, 0.5f);

		g_Croissant[i].spd = 0.0f;			// �ړ��X�s�[�h�N���A
		g_Croissant[i].size = DOT_SIZE;	// �����蔻��̑傫��

		// ���f���̃f�B�t���[�Y��ۑ����Ă����B�F�ς��Ή��ׁ̈B
		GetModelDiffuse(&g_Croissant[0].model, &g_Croissant[0].diffuse[0]);

		XMFLOAT3 crpos = g_Croissant[i].pos;
		crpos.y -= (DOT_OFFSET_Y - 0.1f);
		g_Croissant[i].shadowIdx = CreateShadow(crpos, DOT_SHADOW_SIZE, DOT_SHADOW_SIZE);

		g_Croissant[i].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		g_Croissant[i].tbl_adr = NULL;			// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		g_Croissant[i].tbl_size = 0;			// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		g_Croissant[i].use = true;				// true:�����Ă�


		//// cookies
		//g_Cookies[0].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		//g_Cookies[0].tbl_adr = move_tbl;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Cookies[0].tbl_size = sizeof(move_tbl) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		//// hotdog
		//g_Hotdog[0].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		//g_Hotdog[0].tbl_adr = move_tbl;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Hotdog[0].tbl_size = sizeof(move_tbl2) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		//// cherry
		//g_Cherry[0].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		//g_Cherry[0].tbl_adr = move_tbl;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Cherry[0].tbl_size = sizeof(move_tbl3) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		//// bread
		//g_Bread[0].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		//g_Bread[0].tbl_adr = move_tbl;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Bread[0].tbl_size = sizeof(move_tbl4) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

		//// croissant
		//g_Croissant[0].move_time = 0.0f;		// ���`��ԗp�̃^�C�}�[���N���A
		//g_Croissant[0].tbl_adr = move_tbl;		// �Đ�����A�j���f�[�^�̐擪�A�h���X���Z�b�g
		//g_Croissant[0].tbl_size = sizeof(move_tbl5) / sizeof(INTERPOLATION_DATA);	// �Đ�����A�j���f�[�^�̃��R�[�h�����Z�b�g

	}

	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitDot(void)
{
	if (g_Load == FALSE) return;

	for (int i = 0; i < MAX_DOT; i++)
	{
		if (g_Cookies[i].load)
		{
			UnloadModel(&g_Cookies[i].model);
			g_Cookies[i].load = false;
		}
		if (g_Hotdog[i].load)
		{
			UnloadModel(&g_Hotdog[i].model);
			g_Hotdog[i].load = false;
		}
		if (g_Cherry[i].load)
		{
			UnloadModel(&g_Cherry[i].model);
			g_Cherry[i].load = false;
		}
		if (g_Bread[i].load)
		{
			UnloadModel(&g_Bread[i].model);
			g_Bread[i].load = false;
		}
		if (g_Croissant[i].load)
		{
			UnloadModel(&g_Croissant[i].model);
			g_Croissant[i].load = false;
		}
	}

	g_Load = FALSE;

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateDot(void)
{
	// �G�l�~�[�𓮂����ꍇ�́A�e�����킹�ē���������Y��Ȃ��悤�ɂˁI
	for (int i = 0; i < MAX_DOT; i++)
	{
		// COOKIES
		if (g_Cookies[i].use == true)			// ���̃G�l�~�[���g���Ă���H
		{										// Yes
			if (g_Cookies[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
			{									// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_Cookies[i].move_time;
				float	time = g_Cookies[i].move_time - index;
				int		size = g_Cookies[i].tbl_size;

				float dt = 1.0f / g_Cookies[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_Cookies[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_Cookies[i].move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_Cookies[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_Cookies[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_Cookies[i].pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_Cookies[i].tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_Cookies[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_Cookies[i].rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_Cookies[i].tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_Cookies[i].tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_Cookies[i].scl, s0 + scl * time);

			}

			// ���C�L���X�g���đ����̍��������߂�
			XMFLOAT3 HitPosition;		// ��_
			XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
			bool ans = RayHitField(g_Cookies[i].pos, &HitPosition, &Normal);
			if (ans)
			{
				g_Cookies[i].pos.y = HitPosition.y + DOT_OFFSET_Y;
			}
			else
			{
				g_Cookies[i].pos.y = DOT_OFFSET_Y;
				Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			}

			// �e���G�l�~�[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_Cookies[i].pos;
			pos.y -= (DOT_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Cookies[i].shadowIdx, pos);
		}


		// HOTDOG
		if (g_Hotdog[i].use == true)			// ���̃G�l�~�[���g���Ă���H
		{										// Yes
			if (g_Hotdog[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
			{									// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_Hotdog[i].move_time;
				float	time = g_Hotdog[i].move_time - index;
				int		size = g_Hotdog[i].tbl_size;

				float dt = 1.0f / g_Hotdog[i].tbl_adr[index].frame;		// 1�t���[���Ői�߂鎞��
				g_Hotdog[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_Hotdog[i].move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_Hotdog[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_Hotdog[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_Hotdog[i].pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_Hotdog[i].tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_Hotdog[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_Hotdog[i].rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_Hotdog[i].tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_Hotdog[i].tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_Hotdog[i].scl, s0 + scl * time);

			}

			// ���C�L���X�g���đ����̍��������߂�
			XMFLOAT3 HitPosition;		// ��_
			XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
			bool ans = RayHitField(g_Hotdog[i].pos, &HitPosition, &Normal);
			if (ans)
			{
				g_Hotdog[i].pos.y = HitPosition.y + DOT_OFFSET_Y;
			}
			else
			{
				g_Hotdog[i].pos.y = DOT_OFFSET_Y;
				Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			}

			// �e���G�l�~�[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_Hotdog[i].pos;
			pos.y -= (DOT_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Hotdog[i].shadowIdx, pos);
		}


		// CHERRY
		if (g_Cherry[i].use == true)			// ���̃G�l�~�[���g���Ă���H
		{										// Yes
			if (g_Cherry[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
			{									// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_Cherry[i].move_time;
				float	time = g_Cherry[i].move_time - index;
				int		size = g_Cherry[i].tbl_size;

				float dt = 1.0f / g_Cherry[i].tbl_adr[index].frame;		// 1�t���[���Ői�߂鎞��
				g_Cherry[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_Cherry[i].move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_Cherry[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_Cherry[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_Cherry[i].pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_Cherry[i].tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_Cherry[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_Cherry[i].rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_Cherry[i].tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_Cherry[i].tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_Cherry[i].scl, s0 + scl * time);

			}

			// ���C�L���X�g���đ����̍��������߂�
			XMFLOAT3 HitPosition;		// ��_
			XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
			bool ans = RayHitField(g_Cherry[i].pos, &HitPosition, &Normal);
			if (ans)
			{
				g_Cherry[i].pos.y = HitPosition.y + DOT_OFFSET_Y;
			}
			else
			{
				g_Cherry[i].pos.y = DOT_OFFSET_Y;
				Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			}

			// �e���G�l�~�[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_Cherry[i].pos;
			pos.y -= (DOT_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Cherry[i].shadowIdx, pos);
		}


		// BREAD
		if (g_Bread[i].use == true)				// ���̃G�l�~�[���g���Ă���H
		{										// Yes
			if (g_Bread[i].tbl_adr != NULL)		// ���`��Ԃ����s����H
			{									// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_Bread[i].move_time;
				float	time = g_Bread[i].move_time - index;
				int		size = g_Bread[i].tbl_size;

				float dt = 1.0f / g_Bread[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_Bread[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_Bread[i].move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_Bread[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_Bread[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_Bread[i].pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_Bread[i].tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_Bread[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_Bread[i].rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_Bread[i].tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_Bread[i].tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_Bread[i].scl, s0 + scl * time);

			}

			// ���C�L���X�g���đ����̍��������߂�
			XMFLOAT3 HitPosition;		// ��_
			XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
			bool ans = RayHitField(g_Bread[i].pos, &HitPosition, &Normal);
			if (ans)
			{
				g_Bread[i].pos.y = HitPosition.y + DOT_OFFSET_Y;
			}
			else
			{
				g_Bread[i].pos.y = DOT_OFFSET_Y;
				Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			}

			// �e���G�l�~�[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_Bread[i].pos;
			pos.y -= (DOT_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Bread[i].shadowIdx, pos);
		}


		// Croissant
		if (g_Croissant[i].use == true)			// ���̃G�l�~�[���g���Ă���H
		{										// Yes
			if (g_Croissant[i].tbl_adr != NULL)	// ���`��Ԃ����s����H
			{									// ���`��Ԃ̏���
				// �ړ�����
				int		index = (int)g_Croissant[i].move_time;
				float	time = g_Croissant[i].move_time - index;
				int		size = g_Croissant[i].tbl_size;

				float dt = 1.0f / g_Croissant[i].tbl_adr[index].frame;	// 1�t���[���Ői�߂鎞��
				g_Croissant[i].move_time += dt;							// �A�j���[�V�����̍��v���Ԃɑ���

				if (index > (size - 2))	// �S�[�����I�[�o�[���Ă�����A�ŏ��֖߂�
				{
					g_Croissant[i].move_time = 0.0f;
					index = 0;
				}

				// ���W�����߂�	X = StartX + (EndX - StartX) * ���̎���
				XMVECTOR p1 = XMLoadFloat3(&g_Croissant[i].tbl_adr[index + 1].pos);	// ���̏ꏊ
				XMVECTOR p0 = XMLoadFloat3(&g_Croissant[i].tbl_adr[index + 0].pos);	// ���݂̏ꏊ
				XMVECTOR vec = p1 - p0;
				XMStoreFloat3(&g_Croissant[i].pos, p0 + vec * time);

				// ��]�����߂�	R = StartX + (EndX - StartX) * ���̎���
				XMVECTOR r1 = XMLoadFloat3(&g_Croissant[i].tbl_adr[index + 1].rot);	// ���̊p�x
				XMVECTOR r0 = XMLoadFloat3(&g_Croissant[i].tbl_adr[index + 0].rot);	// ���݂̊p�x
				XMVECTOR rot = r1 - r0;
				XMStoreFloat3(&g_Croissant[i].rot, r0 + rot * time);

				// scale�����߂� S = StartX + (EndX - StartX) * ���̎���
				XMVECTOR s1 = XMLoadFloat3(&g_Croissant[i].tbl_adr[index + 1].scl);	// ����Scale
				XMVECTOR s0 = XMLoadFloat3(&g_Croissant[i].tbl_adr[index + 0].scl);	// ���݂�Scale
				XMVECTOR scl = s1 - s0;
				XMStoreFloat3(&g_Croissant[i].scl, s0 + scl * time);

			}

			// ���C�L���X�g���đ����̍��������߂�
			XMFLOAT3 HitPosition;		// ��_
			XMFLOAT3 Normal;			// �Ԃ������|���S���̖@���x�N�g���i�����j
			bool ans = RayHitField(g_Croissant[i].pos, &HitPosition, &Normal);
			if (ans)
			{
				g_Croissant[i].pos.y = HitPosition.y + DOT_OFFSET_Y;
			}
			else
			{
				g_Croissant[i].pos.y = DOT_OFFSET_Y;
				Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			}

			// �e���G�l�~�[�̈ʒu�ɍ��킹��
			XMFLOAT3 pos = g_Croissant[i].pos;
			pos.y -= (DOT_OFFSET_Y - 0.1f);
			SetPositionShadow(g_Croissant[i].shadowIdx, pos);
		}

	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawDot(void)
{
	XMMATRIX mtxScl, mtxRot, mtxTranslate, mtxWorld;

	// �J�����O����
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_DOT; i++)
	{
		// Cookies
		if (g_Cookies[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Cookies[i].scl.x, g_Cookies[i].scl.y, g_Cookies[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Cookies[i].rot.x, g_Cookies[i].rot.y + XM_PI, g_Cookies[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Cookies[i].pos.x, g_Cookies[i].pos.y, g_Cookies[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Cookies[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Cookies[i].model);


		// Hotdog
		if (g_Hotdog[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Hotdog[i].scl.x, g_Hotdog[i].scl.y, g_Hotdog[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Hotdog[i].rot.x, g_Hotdog[i].rot.y + XM_PI, g_Hotdog[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Hotdog[i].pos.x, g_Hotdog[i].pos.y, g_Hotdog[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Hotdog[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Hotdog[i].model);


		// Cherry
		if (g_Cherry[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Cherry[i].scl.x, g_Cherry[i].scl.y, g_Cherry[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Cherry[i].rot.x, g_Cherry[i].rot.y + XM_PI, g_Cherry[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Cherry[i].pos.x, g_Cherry[i].pos.y, g_Cherry[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Cherry[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Cherry[i].model);


		// Bread
		if (g_Bread[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Bread[i].scl.x, g_Bread[i].scl.y, g_Bread[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Bread[i].rot.x, g_Bread[i].rot.y + XM_PI, g_Bread[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Bread[i].pos.x, g_Bread[i].pos.y, g_Bread[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Bread[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Bread[i].model);


		// Croissant
		if (g_Croissant[i].use == false) continue;

		// ���[���h�}�g���b�N�X�̏�����
		mtxWorld = XMMatrixIdentity();

		// �X�P�[���𔽉f
		mtxScl = XMMatrixScaling(g_Croissant[i].scl.x, g_Croissant[i].scl.y, g_Croissant[i].scl.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxScl);

		// ��]�𔽉f
		mtxRot = XMMatrixRotationRollPitchYaw(g_Croissant[i].rot.x, g_Croissant[i].rot.y + XM_PI, g_Croissant[i].rot.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxRot);

		// �ړ��𔽉f
		mtxTranslate = XMMatrixTranslation(g_Croissant[i].pos.x, g_Croissant[i].pos.y, g_Croissant[i].pos.z);
		mtxWorld = XMMatrixMultiply(mtxWorld, mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		SetWorldMatrix(&mtxWorld);

		XMStoreFloat4x4(&g_Croissant[i].mtxWorld, mtxWorld);

		// ���f���`��
		DrawModel(&g_Croissant[i].model);


	}

	// �J�����O�ݒ��߂�
	SetCullingMode(CULL_MODE_BACK);

}

//=============================================================================
// �G�l�~�[�̎擾
//=============================================================================
DOT* GetCookies(void)
{
	return &g_Cookies[0];
}
DOT* GetHotdog(void)
{
	return &g_Hotdog[0];
}
DOT* GetCherry(void)
{
	return &g_Cherry[0];
}
DOT* GetBread(void)
{
	return &g_Bread[0];
}
DOT* GetCroissant(void)
{
	return &g_Croissant[0];
}
