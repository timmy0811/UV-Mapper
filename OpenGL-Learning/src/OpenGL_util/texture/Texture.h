#pragma once

#include "../core/Renderer.h"

#include "vendor/stb_image/stb_image.h"
#include "vendor/stb_image/stb_image_write.h"

enum class TextureType { DIFFUSE, SPECULAR, SHINE, NORMAL, HEIGHT, DEFAULT };

class Texture
{
private:
	mutable int m_BoundID = -1;
	unsigned int m_RendererID;
	std::string m_Filepath;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;

	TextureType m_Type;

public:
	Texture(const std::string& path, const bool flipUV = false, const bool freeOnInit = false);
	~Texture();

	int Bind(const unsigned int slot = 0) const;
	void Unbind();

	const glm::vec4 getPixelRGBA(const glm::vec2& pixel);

	inline const std::string& GetPath() const { return m_Filepath; }
	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	inline int GetRendererID() const { return m_RendererID; }
	inline int GetBoundPort() const { return m_BoundID; }
	inline int GetChannels() const { return m_BPP; }
	inline unsigned char* GetDataAllocator() { return m_LocalBuffer; }

	void SetType(TextureType type) { m_Type = type; }
	inline TextureType GetType() { return m_Type; }
};