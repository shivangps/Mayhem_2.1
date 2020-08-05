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
    float3 fragPos = float3(0.0f, 0.0f, 0.0f);
    float3 color = float3(0.0f, 0.0f, 0.0f);
    float4 normal = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
    fragPos = gFragmentPosTexture.Sample(mainSampler, IN.texcoord);
    color = gAlbedoTexture.Sample(mainSampler, IN.texcoord);
    normal = gNormalTexture.Sample(mainSampler, IN.texcoord);
    
    float3 lightPosition = float3(2.0f, 2.0f, -2.0f);
    
    float3 lightDirection = normalize(lightPosition - fragPos);
    float diffuseFactor = max(dot(normal.xyz, lightDirection), 0.1f);
    
    OUT.color = float4(color * diffuseFactor ,1.0f);
}