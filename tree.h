//=============================================================================
//
// �؏��� [tree.h]
// Author : 
//
//=============================================================================
#pragma once
struct SIGN
{
	XMFLOAT3			pos;				// �ʒu
	XMFLOAT3			rot;				// ��]
	XMFLOAT3			scl;				// �X�P�[��
	MATERIAL			material;			// �}�e���A��
	float				fWidth;				// ��
	float				fHeight;			// ����
	int					nIdxShadow;			// �eID
	bool				bUse;				// �g�p���Ă��邩�ǂ���

	INTERPOLATION_DATA* tbl_adr;			// �A�j���f�[�^�̃e�[�u���擪�A�h���X
	int					tbl_size;			// �o�^�����e�[�u���̃��R�[�h����
	float				move_time;			// ���s����
};


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitTree(void);
void UninitTree(void);
void UpdateTree(void);
void DrawTree(void);

int SetTree(XMFLOAT3 pos, float fWidth, float fHeight, XMFLOAT4 col);


