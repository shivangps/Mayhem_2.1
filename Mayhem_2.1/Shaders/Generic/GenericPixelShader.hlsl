struct ps_in
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 fragPosition : FRAGPOSITION;
    float2 texCoord : TEXCOORD;
};

struct ps_out
{
    float3 fragPos : SV_TARGET0;
    float3 color : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 specgloss : SV_TARGET3;
};

cbuffer ConstantBufferData : register(b0)
{
    float saturation;
}

Texture2D mainTexture : register(t0);
SamplerState mainSampler : register(s0);

void main(in ps_in IN, out ps_out OUT)
{
    float3 normal = normalize(IN.normal);
    float3 coreColor = mainTexture.Sample(mainSampler, IN.texCoord).rgb;
    
    OUT.fragPos = IN.fragPosition;
    OUT.normal = float4(normal, 1.0f);
    OUT.color = coreColor;
    OUT.specgloss = float4(1.0f, 1.0f, 1.0f, 1.0f);
}