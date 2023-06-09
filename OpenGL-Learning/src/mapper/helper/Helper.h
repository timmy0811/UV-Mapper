#pragma once

#include "OpenGL_util/core/Shader.h"
#include "vendor/stb_image/stb_image.h"

namespace Helper {
	// Structs
	struct Vec2_4 {
		glm::vec2 u0;
		glm::vec2 u1;
		glm::vec2 u2;
		glm::vec2 u3;
	};

	struct SymbolInformation {
		Vec2_4 uv;
		unsigned int width;
	};

	struct ShaderPackage {
		Shader* shaderBlockStatic;

		~ShaderPackage() {
			delete shaderBlockStatic;
		}
	};

	struct ImageInformation {
		glm::ivec2 Size;
		std::string Path;
		int BitDepth;
	};

	struct Sprite {
		Sprite(const std::string& path, const glm::vec2& position, const glm::vec2& size, const bool flipUV = false) {
			this->Path = path;
			this->Position = position;
			this->Size = size;
			this->FlipUvs = flipUV;

			Uvs.u0 = { 0.f, 1.f };
			Uvs.u1 = { 1.f, 1.f };
			Uvs.u2 = { 1.f, 0.f };
			Uvs.u3 = { 0.f, 0.f };
		}

		Sprite(const std::string& path, const glm::vec2& position, const glm::vec2& size, Helper::Vec2_4 uvs, const bool flipUV = false) {
			this->Path = path;
			this->Position = position;
			this->Size = size;
			this->FlipUvs = flipUV;

			Uvs = uvs;
		}

		Sprite() {};

		unsigned int Id;
		std::string Path;
		glm::vec2 Position;
		glm::vec2 Size;
		Helper::Vec2_4 Uvs;

		ImageInformation Information;

		bool FlipUvs;

		/// <summary>
		/// Loads image into memory and retrieves its (meta)data.
		/// </summary>
		/// <returns>ImageInformation struct</returns>
		ImageInformation getImageInformation() {
			if (Information.Path.size() == 0) {
				Information.Path = Path;
				unsigned char* buffer = stbi_load(Path.c_str(), &Information.Size.x, &Information.Size.y, &Information.BitDepth, 4);
				if (buffer) stbi_image_free(buffer);
			}

			return Information;
		}
	};

	// Functions
	static int mapRGBToInt(const glm::vec3& color)
	{
		int r = int(color.r * 255) << 16;
		int g = int(color.g * 255) << 8;
		int b = int(color.b * 255);

		return r | g | b;
	}

	struct UVlabels {
		char uv0X[10];
		char uv0Y[10];

		char uv1X[10];
		char uv1Y[10];

		char uv2X[10];
		char uv2Y[10];

		char uv3X[10];
		char uv3Y[10];
	};
}