struct ps_in
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

struct ps_out
{
    float4 color : SV_TARGET;
};

Texture2D gFragmentPosTexture : register(t0); //3
Texture2D gAlbedoTexture : register(t1); //3
Texture2D gNormalTexture : register(t2); //4
Texture2D gSpecularGlossTexture : register(t3); //4
Texture2D gDepth : register(t4); //2

SamplerState mainSampler : register(s0);

void main(in ps_in IN, out ps_out OUT)
{
    float3 fragPos = gFragmentPosTexture.Sample(mainSampler, IN.texcoord).xyz;
    float3 color = gAlbedoTexture.Sample(mainSampler, IN.texcoord).rgb;
    float3 normal = gNormalTexture.Sample(mainSampler, IN.texcoord).rgb;
    
    OUT.color = float4(color, 1.0f);

}