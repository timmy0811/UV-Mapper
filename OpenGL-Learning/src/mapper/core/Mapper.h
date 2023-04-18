#pragma once

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include "mapper/helper/CustomRenderer.h"

#define BG_TILE_WIDTH 32.f
#define FONT_SIZE 2.f

class Mapper
{
private:
	Helper::LineRenderer m_LineRenderer;
	Helper::SpriteRenderer m_BackgroundRenderer;
	Helper::FontRenderer m_FontRenderer;

	const float m_BGwidthTiles = 128.f;

	glm::vec2 m_ViewOffset;
	glm::mat4 m_Projection;

	bool m_InTranslationMode;

	// Input
	double m_LastX = 400, m_LastY = 300;

	inline static double s_MouseX = 0;
	inline static double s_MouseY = 0;

	void ProcessMouse(GLFWwindow* window);
	static void OnMouseCallback(GLFWwindow* window, double xpos, double ypos);
	static void OnScrollCallback(GLFWwindow* window, double xpos, double ypos);

public:
	explicit Mapper();
	~Mapper();

	void OnUpdate();
	void OnRender();
	void OnInput(GLFWwindow* window);
};

