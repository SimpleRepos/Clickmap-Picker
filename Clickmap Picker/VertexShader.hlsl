struct Vertex {
  float3 pos : POSITION;
  float2 tex : TEXCOORD;
};

struct Output {
  float4 pos : SV_POSITION;
  float2 tex : TEXCOORD;
};

static const int2 vpDims = { 800, 600 };

Output main( Vertex iv ) {
  Output ov;
  ov.pos.x = ((2 * iv.pos.x) - vpDims.x) / vpDims.x;
  ov.pos.y = ((2 * iv.pos.y) - vpDims.y) / vpDims.y;
  ov.pos.z = iv.pos.z / 10;
  ov.pos.w = 1;

  ov.tex = iv.tex;
	return ov;
}
