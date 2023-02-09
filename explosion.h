//=============================================================================
//
// �������� [explosion.h]
// Author : ���@�E��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
enum EXPLOSION_TYPE
{
	DIE_EXPLO,
	OFFGROUND_EXPLO,

	EXPLOSION_TYPE_NUM,
};

struct EXPLOSION
{
	XMFLOAT3	pos;			// �ʒu
	XMFLOAT3	scl;			// �X�P�[��
	MATERIAL	material;		// �}�e���A��
	float		fWidth;			// ��
	float		fHeight;		// ����
	int			nIdxShadow;		// �eID
	bool		bUse;			// �g�p���Ă��邩�ǂ���
	float		countAnim;		// animation count
	float		animFlame;		// animation flame 

	int			type;			// �����^�C�v
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitExplosion(void);
void UninitExplosion(void);
void UpdateExplosion(void);
void DrawExplosion(void);

EXPLOSION* GetExplosion(void);

int SetExplosion(XMFLOAT3 pos, float fWidth, float fHeight, int type);


