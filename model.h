//=============================================================================
//
// ���f���̏��� [model.h]
// Author :
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*********************************************************
// �\����
//*********************************************************

// �}�e���A���\����

#define MODEL_MAX_MATERIAL		(16)		// �P���f����Max�}�e���A����

struct DX11_MODEL_MATERIAL
{
	MATERIAL					Material;
	ID3D11ShaderResourceView* Texture;
};

// �`��T�u�Z�b�g�\����
struct DX11_SUBSET
{
	unsigned short	StartIndex;
	unsigned short	IndexNum;
	DX11_MODEL_MATERIAL	Material;
};

struct DX11_MODEL
{
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;

	VERTEX_3D* VertexArray;
	unsigned short* IndexArray;

	DX11_SUBSET* SubsetArray;
	unsigned short	SubsetNum;
};



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void LoadModel(char* FileName, DX11_MODEL* Model);
void UnloadModel(DX11_MODEL* Model);
void DrawModel(DX11_MODEL* Model);
void DrawInstanceModel(DX11_MODEL* Model, int InsanceNum, ID3D11Buffer* instanceBuffer, int shaderMode);

// ���f���̃}�e���A���̃f�B�t���[�Y���擾����BMax16���ɂ��Ă���
void GetModelDiffuse(DX11_MODEL* Model, XMFLOAT4* diffuse);

// ���f���̎w��}�e���A���̃f�B�t���[�Y���Z�b�g����B
void SetModelDiffuse(DX11_MODEL* Model, int mno, XMFLOAT4 diffuse);

bool RayHitModel(DX11_MODEL* Model, XMFLOAT4X4 mtxWorld, XMFLOAT3 rayPos, float rayLength, XMFLOAT3 rayDirection, XMFLOAT3* hitPos, XMFLOAT3* normal);

