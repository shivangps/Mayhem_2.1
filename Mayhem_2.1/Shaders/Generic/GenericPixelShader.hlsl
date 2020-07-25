struct ps_in
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 fragPosition : FRAGPOSITION;
    float2 texCoord : TEXCOORD;
};

struct ps_out
{
    float4 color : SV_TARGET;
};

cbuffer ConstantBufferData : register(b0)
{
    float saturation;
}

Texture2D mainTexture : register(t0);
SamplerState mainSampler : register(s0);

void main(in ps_in IN, out ps_out OUT)
{
    float3 lightPos = float3(2.0f, 2.0f, -2.0f);
    float3 normal = normalize(IN.normal);
    float4 coreColor = mainTexture.Sample(mainSampler, IN.texCoord);
    
    float3 lightDirection = normalize(lightPos - IN.fragPosition);
    float diffuseFactor = max(dot(normal, lightDirection), saturation);
    
    OUT.color = coreColor * (diffuseFactor + saturation);
}