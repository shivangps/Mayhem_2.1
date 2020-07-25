struct vs_in
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

struct vs_out
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 fragPosition : FRAGPOSITION;
    float2 texCoord : TEXCOORD;
};

cbuffer TransformationData : register(b1)
{
    float4x4 MVPmat;
    float4x4 Modelmat;
    float4x4 InvTrpModelMat;
};

void main(in vs_in IN, out vs_out OUT)
{
    OUT.position = mul(float4(IN.position, 1.0f), MVPmat);
    OUT.normal = mul(IN.normal, (float3x3) InvTrpModelMat);
    OUT.fragPosition = mul(float4(IN.position, 1.0f), Modelmat);
    OUT.texCoord = IN.texCoord;
}