#pragma once

#include <cstdint>

struct BloomRenderer
{

	BloomRenderer() : mInit(false) { Init(); }
	~BloomRenderer() = default;
	bool Init();
	void Destroy();
	void RenderBloomTexture(float filterRadius);
	unsigned int getBloomTexture();
	unsigned int BloomMip_i(int index);
  void Render();
  void Bind() const;
  void UnBind() const;

private:
  uint32_t hdrFBO, colorBuffers[2], rboDepth, pingpongFBO[2], pingpongColorbuffers[2];
  
	void RenderDownsamples(unsigned int srcTexture);
	void RenderUpsamples(float filterRadius);
  void Resize(int newWidth, int newHeight);
  bool mInit;
	bool mKarisAverageOnDownsample = true;
};
