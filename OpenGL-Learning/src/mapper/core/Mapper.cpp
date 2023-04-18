#include "Mapper.h"

void Mapper::ProcessMouse(GLFWwindow* window)
{
	static bool init = true;
	if (init) {
		m_LastX = s_MouseX;
		m_LastY = s_MouseY;
		init = false;
	}

	double xoffset = s_MouseX - m_LastX;
	double yoffset = m_LastY - s_MouseY;

	m_LastX = s_MouseX;
	m_LastY = s_MouseY;

	static bool keyPressed1 = false;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && !keyPressed1)
	{
		keyPressed1 = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		m_InTranslationMode = true;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE && keyPressed1) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		keyPressed1 = false;
		m_InTranslationMode = false;
	}

	if (m_InTranslationMode) {
		m_ViewOffset += glm::vec2((float)xoffset, (float)yoffset);
	}
}

void Mapper::OnMouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	s_MouseX = float(xpos);
	s_MouseY = float(ypos);
}

void Mapper::OnScrollCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (ypos < 0.f) LOGC("Scroll down");
	else if (ypos > 0.f) LOGC("Scroll up");
}

Mapper::Mapper()
	:m_BackgroundRenderer(1, "res/shader/sprite/shader_sprite_bg.vert", "res/shader/sprite/shader_sprite_bg.frag"),
	m_LineRenderer(2, "res/shader/line/shader_single_color.vert", "res/shader/line/shader_single_color.frag"),
	m_FontRenderer("res/ascii_gui_0.png", "res/font.yaml", 128, false)
{
	m_Projection = glm::ortho(0.0f, (float)conf.WIN_WIDTH, 0.0f, (float)conf.WIN_HEIGHT, -1.0f, 1.0f);

	m_BackgroundRenderer.AddSprite("res/background.png", { -BG_TILE_WIDTH, -BG_TILE_WIDTH }, { BG_TILE_WIDTH, BG_TILE_WIDTH });
	m_BackgroundRenderer.getShaderPtr()->SetUniform1f("u_WidthTiles", m_BGwidthTiles);
	m_BackgroundRenderer.getShaderPtr()->SetUniform1f("u_TileWidth", BG_TILE_WIDTH);
	
	constexpr float center = 4.f;
	m_LineRenderer.AddLine({-1000.f, conf.WIN_HEIGHT - center * BG_TILE_WIDTH}, {3000.f, conf.WIN_HEIGHT - center * BG_TILE_WIDTH }, {1.f, 0.f, 0.f, 1.f});
	m_LineRenderer.AddLine({ center * BG_TILE_WIDTH, -1000.f}, { center * BG_TILE_WIDTH, 3000.f }, { 0.f, 0.f, 1.f, 1.f });

	m_FontRenderer.PrintMultilineText("(0.0, 0.0)", { center * BG_TILE_WIDTH + 15.f, conf.WIN_HEIGHT - center * BG_TILE_WIDTH + FONT_SIZE * 12.f }, FONT_SIZE, { 0.15, 0.15, 0.15, 0.5f });
}

Mapper::~Mapper()
{
}

void Mapper::OnUpdate()
{
	m_LineRenderer.shader->SetUniformMat4f("u_MVP", m_Projection * glm::translate(glm::mat4(1.f), glm::vec3(m_ViewOffset, 0.f)));
	m_BackgroundRenderer.SetTranslation({ m_ViewOffset.x, m_ViewOffset.y, 0.f });
	m_FontRenderer.SetTranslation({ m_ViewOffset.x, m_ViewOffset.y, 0.f });
}

void Mapper::OnRender()
{
	size_t count = m_BGwidthTiles * (0.5 * m_BGwidthTiles);
	m_BackgroundRenderer.DrawInstanced(count);
	m_FontRenderer.Draw();
	m_LineRenderer.Draw();
}

void Mapper::OnInput(GLFWwindow* window)
{
	glfwGetCursorPos(window, &s_MouseX, &s_MouseY);
	glfwSetScrollCallback(window, OnScrollCallback);
	ProcessMouse(window);
}
