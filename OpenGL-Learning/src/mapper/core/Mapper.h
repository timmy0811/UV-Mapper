#pragma once

#include "mapper/helper/CustomRenderer.h"

#define BG_TILE_WIDTH 64.f

class Mapper
{
private:
	Helper::SpriteRenderer m_BackgroundRenderer;
	const float m_BGwidthTiles = 128.f;

public:
	explicit Mapper();
	~Mapper();

	void OnRender();
};

