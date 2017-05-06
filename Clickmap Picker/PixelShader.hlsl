Texture2D tex : register(t0);
SamplerState samp;

struct Vertex {
  float4 pos : SV_POSITION;
  float2 tex : TEXCOORD;
};

float4 main(Vertex vert) : SV_TARGET {
  float4 texel = tex.Sample(samp, vert.tex);
  
  float testAlpha = texel.a - 0.9f;
  clip(testAlpha);

	return texel;
}
