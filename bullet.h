//=============================================================================
//
// �e���ˏ��� [bullet.h]
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MAX_BULLET		(256)	// �e�ő吔

enum TYPE
{
	LINE,
	BEZIER,
	
	TYPE_NUM,
};

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	XMFLOAT4X4	mtxWorld;		// ���[���h�}�g���b�N�X
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	rot;			// �p�x
	XMFLOAT3	scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	float		spd;			// �ړ���
	float		fWidth;			// ��
	float		fHeight;		// ����
	int			shadowIdx;		// �eID
	bool		use;			// �g�p���Ă��邩�ǂ���

	int			aniFrame;		// �A�j���[�V�����p�t���[��
	int			aniDir;			// �A�j���[�V�����p����

	int			moveType; // �ړ��^�C�v
	XMFLOAT3	beizerPos[4]; // �x�W�F�Ȑ��p�̍��W
	float		beizerTime; // �x�W�F�Ȑ��p�̎���


} BULLET;


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

int SetBullet(XMFLOAT3 pos, XMFLOAT3 rot);
int SetBulletBezier(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 targetPos);

BULLET *GetBullet(void);

