#include "Mapper.h"

Mapper::Mapper()
	:m_BackgroundRenderer(1, "res/shader/sprite/shader_sprite_bg.vert", "res/shader/sprite/shader_sprite_bg.frag")
{
	m_BackgroundRenderer.AddSprite("res/background.png", { -BG_TILE_WIDTH, -BG_TILE_WIDTH }, { BG_TILE_WIDTH, BG_TILE_WIDTH });
	m_BackgroundRenderer.getShaderPtr()->SetUniform1f("u_WidthTiles", m_BGwidthTiles);
	m_BackgroundRenderer.getShaderPtr()->SetUniform1f("u_TileWidth", BG_TILE_WIDTH);
}

Mapper::~Mapper()
{
}

void Mapper::OnRender()
{
	size_t count = m_BGwidthTiles * (0.5 * m_BGwidthTiles);
	m_BackgroundRenderer.DrawInstanced(count);
}
