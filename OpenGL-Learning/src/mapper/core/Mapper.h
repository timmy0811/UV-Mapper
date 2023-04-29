#pragma once

#include <GLEW/glew.h>
#include <GLFW/glfw3.h>

#include "nfd.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include "mapper/helper/CustomRenderer.h"

#define BG_TILE_WIDTH 32.f
#define FONT_SIZE 2.f
#define DIST_TO_CANVAS 200.f
#define CENTER 4.f
#define ZOOM_SPEED 0.055f
#define ZOOM_MAX_IN 20.f
#define SELECT_PXL_SIZE 1.5f

class Mapper
{
private:
	Helper::LineRenderer m_LineRenderer;
	Helper::LineRenderer m_ImageBorderRenderer;
	Helper::LineRenderer m_GridRenderer;

	Helper::SpriteRenderer m_BackgroundRenderer;
	Helper::FontRenderer m_FontRenderer;
	Helper::QuadRenderer m_PixelRenderer;

	Helper::SpriteRenderer m_ImageRenderer;
	glm::vec2 m_ImageSize;

	bool m_ImageOpen;
	float m_GridAlphaMax = 1.f;
	float m_GridAlpha;
	bool m_Flipped;
	bool m_LastFlippedState;

	nfdchar_t* m_WorkingPath = new char[200];

	Helper::Sprite m_OperationImage;

	glm::vec2 m_ViewOffset;
	glm::mat4 m_Projection;
	inline static float m_Zoom;

	bool m_InTranslationMode;

	glm::vec2 m_DynamicViewBorder = { 1000.f, 1000.f };

	// Input
	double m_LastX = 400, m_LastY = 300;

	inline static double s_MouseX = 0;
	inline static double s_MouseY = 0;

	void ProcessMouse(GLFWwindow* window);
	static void OnMouseCallback(GLFWwindow* window, double xpos, double ypos);
	static void OnScrollCallback(GLFWwindow* window, double xpos, double ypos);

	void loadImage(const std::string& path);
	void saveImage(const std::string& path);
	void addGuideLines();

	void drawSelectPixel();

public:
	explicit Mapper();
	~Mapper();

	void OnUpdate();
	void OnRender();
	void OnGuiRender();
	void OnInput(GLFWwindow* window);
};

