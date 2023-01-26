//*****************************************************************************
// �萔�o�b�t�@
//*****************************************************************************

// �}�g���N�X�o�b�t�@
cbuffer WorldBuffer : register(b0)
{
    matrix World;
}

cbuffer ViewBuffer : register(b1)
{
    matrix View;
}

cbuffer ProjectionBuffer : register(b2)
{
    matrix Projection;
}

// �}�e���A���o�b�t�@
struct MATERIAL
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emission;
    float Shininess;
    int noTexSampling;
    float Dummy[2]; //16byte���E�p
};

cbuffer MaterialBuffer : register(b3)
{
    MATERIAL Material;
}

// ���C�g�p�o�b�t�@
struct LIGHT
{
    float4 Direction[5];
    float4 Position[5];
    float4 Diffuse[5];
    float4 Ambient[5];
    float4 Attenuation[5];
    int4 Flags[5];
    int Enable;
    int Dummy[3]; //16byte���E�p
};

cbuffer LightBuffer : register(b4)
{
    LIGHT Light;
}

struct FOG
{
    float4 Distance;
    float4 FogColor;
    int Enable;
    float Dummy[3]; //16byte���E�p
};

// �t�H�O�p�o�b�t�@
cbuffer FogBuffer : register(b5)
{
    FOG Fog;
};

// �����p�o�b�t�@
cbuffer Fuchi : register(b6)
{
    int fuchi;
    int fill[3];
};

cbuffer CameraBuffer : register(b7)
{
    float4 Camera;
};

// ���ԗp�o�b�t�@
cbuffer TimeBuffer : register(b8)
{
    float4 Time;
};


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);



//=============================================================================
// �n�ʒ��_�V�F�[�_
//=============================================================================
struct GV_IN
{
    float4 Position : POSITION0;
    float4 Normal   : NORMAL0;
    float4 Diffuse  : COLOR0;
    float2 TexCoord : TEXCOORD0;
};

struct PS_IN
{
    float4 pos       : SV_POSITION;
    float4 norw      : NORMAL0;
    float2 TexCoord  : TEXCOORD0;
    float4 Diffuse   : COLOR0;
    float4 posw      : POSITION0;
};

PS_IN GroundVS(GV_IN input) 
{
    PS_IN output;
      
    matrix wvp;

    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    
    float4 Xbias = 0.1 * sin(Time.x);        // float4 time �Ȃ̂ŁAx�̂ݐ��l������܂��B
    input.Position.x += Xbias;
    
    output.pos = mul(input.Position, wvp);

    output.norw = normalize(mul(float4(input.Normal.xyz, 0.0f), World));

    output.TexCoord = input.TexCoord;

    output.posw = mul(input.Position, World);

    output.Diffuse = input.Diffuse;
    
    return output;
}



//=============================================================================
// ���˃s�N�Z���V�F�[�_
//=============================================================================

float4 ReflectionPS(PS_IN input) : SV_Target
{
    float3 l;
    float3 n;
    float3 r;
    float3 v;
    float  i;
    
    // input material color
    float4 color;

    color = g_Texture.Sample(g_SamplerState, input.TexCoord);

    color *= input.Diffuse;
    color = color * Material.Diffuse;
    
    l = normalize(Light.Position[0].xyz - input.posw.xyz);
    n = normalize(input.norw.xyz);
    r = 2.0 * n * dot(n, 1) - 1;
    v = normalize(Camera.xyz - input.posw.xyz);
    i = pow(saturate(dot(r, v)), Material.Diffuse.w);
    
    //return color;
    return float4 (i * color.xyz * Light.Diffuse[0].xyz, 1.0);
    
}




