#include "Mapper.h"

Mapper::Mapper()
	:m_BackgroundRenderer(1, "res/shader/sprite/shader_sprite_bg.vert", "res/shader/sprite/shader_sprite_bg.frag"),
	m_LineRenderer(2, "res/shader/line/shader_single_color.vert", "res/shader/line/shader_single_color.frag"),
	m_ImageBorderRenderer(6, "res/shader/line/shader_single_color.vert", "res/shader/line/shader_single_color.frag"),
	m_GridRenderer(1, "res/shader/line/shader_single_color_instanced.vert", "res/shader/line/shader_single_color.frag"),
	m_FontRenderer("res/ascii_gui_0.png", "res/font.yaml", 128, false),
	m_ImageRenderer(1, "res/shader/sprite/shader_sprite.vert", "res/shader/sprite/shader_sprite.frag"),
	m_PixelRenderer(2, "res/shader/line/shader_single_color.vert", "res/shader/line/shader_single_color.frag")
{
	m_Projection = glm::ortho(0.0f, (float)conf.WIN_WIDTH, 0.0f, (float)conf.WIN_HEIGHT, -1.0f, 1.0f);

	m_BackgroundRenderer.AddSprite("res/background.png", { -DIST_TO_CANVAS - BG_TILE_WIDTH, conf.WIN_HEIGHT + DIST_TO_CANVAS - BG_TILE_WIDTH }, { BG_TILE_WIDTH, BG_TILE_WIDTH });
	m_BackgroundRenderer.getShaderPtr()->SetUniform1f("u_TileWidth", BG_TILE_WIDTH);

	addGuideLines();

	m_FontRenderer.PrintMultilineText("(0.0, 0.0)", { CENTER * BG_TILE_WIDTH + 15.f, conf.WIN_HEIGHT - CENTER * BG_TILE_WIDTH + FONT_SIZE * 12.f }, FONT_SIZE, { 0.15, 0.15, 0.15, 0.5f });
	m_Zoom = 1.f;
}

Mapper::~Mapper()
{
	delete[] m_WorkingPath;
}

void Mapper::OnUpdate()
{
	static float zoom = 0.5f;
	zoom += 0.1f * (1.f / 60.f);

	// Apply Transforms
	m_LineRenderer.shader->Bind();
	m_LineRenderer.shader->SetUniformMat4f("u_MVP", m_Projection * glm::scale(glm::mat4(1.f), glm::vec3(m_Zoom)) * glm::translate(glm::mat4(1.f), glm::vec3(m_ViewOffset, 0.f)));

	m_ImageBorderRenderer.shader->Bind();
	m_ImageBorderRenderer.shader->SetUniformMat4f("u_MVP", m_Projection * glm::scale(glm::mat4(1.f), glm::vec3(m_Zoom)) * glm::translate(glm::mat4(1.f), glm::vec3(m_ViewOffset, 0.f)));

	m_GridRenderer.shader->Bind();
	m_GridRenderer.shader->SetUniform1f("u_TexelWidth", 1.f);
	glm::mat4 m = m_Projection * glm::scale(glm::mat4(1.f), glm::vec3(m_Zoom)) * glm::translate(glm::mat4(1.f), glm::vec3(m_ViewOffset, 0.f));
	m_GridRenderer.shader->SetUniformMat4f("u_MVP", m);

	m_PixelRenderer.SetTranslation({ m_ViewOffset.x, m_ViewOffset.y, 0.f }, glm::vec3(m_Zoom));

	//m_BackgroundRenderer.SetTranslation({ m_ViewOffset.x, m_ViewOffset.y, 0.f }, glm::vec3(m_Zoom));		// Background transforming?
	m_FontRenderer.SetTranslation({ m_ViewOffset.x, m_ViewOffset.y, 0.f }, glm::vec3(m_Zoom));
	m_ImageRenderer.SetTranslation({ m_ViewOffset.x, m_ViewOffset.y, 0.f }, glm::vec3(m_Zoom));

	// Fade Grid
	float fac = std::clamp((float)((m_Zoom - 3.f) / (ZOOM_MAX_IN / 2.f)), 0.f, 1.f);
	m_GridAlpha = fac * m_GridAlphaMax;
}

void Mapper::OnRender()
{
	// Render Instanced Background
	float countBGTilesX = std::ceil((DIST_TO_CANVAS + m_DynamicViewBorder.x + conf.WIN_WIDTH + BG_TILE_WIDTH) / BG_TILE_WIDTH);
	float countBGTilesY = std::ceil((DIST_TO_CANVAS + m_DynamicViewBorder.y + conf.WIN_HEIGHT) / BG_TILE_WIDTH);
	size_t count = countBGTilesX * countBGTilesY;

	m_BackgroundRenderer.getShaderPtr()->Bind();
	m_BackgroundRenderer.getShaderPtr()->SetUniform1f("u_WidthTiles", countBGTilesX);

	m_BackgroundRenderer.DrawInstanced(count);
	m_FontRenderer.Draw();
	m_ImageRenderer.Draw();

	// Render Grid
	if (m_ImageOpen && m_Zoom > 3.f) {
		m_GridRenderer.shader->Bind();

		// Horizontal
		m_GridRenderer.vb->Empty();
		m_GridRenderer.AddLine({ CENTER * BG_TILE_WIDTH, CENTER * BG_TILE_WIDTH + 1.f }, { CENTER * BG_TILE_WIDTH + m_ImageSize.x, CENTER * BG_TILE_WIDTH + 1.f }, { 1.f, 1.f, 1.f, m_GridAlpha * 0.5 });
		m_GridRenderer.shader->SetUniform1f("u_X", 0.f);
		m_GridRenderer.DrawInstanced(m_ImageSize.y - 1.f);

		// Vertical
		m_GridRenderer.vb->Empty();
		m_GridRenderer.AddLine({ CENTER * BG_TILE_WIDTH + 1.f, CENTER * BG_TILE_WIDTH }, { CENTER * BG_TILE_WIDTH + 1.f, CENTER * BG_TILE_WIDTH + m_ImageSize.y }, { 1.f, 1.f, 1.f, m_GridAlpha * 0.5 });
		m_GridRenderer.shader->SetUniform1f("u_X", 1.f);
		m_GridRenderer.DrawInstanced(m_ImageSize.x - 1.f);
	}

	m_LineRenderer.Draw(3.f);
	m_ImageBorderRenderer.Draw();

	drawSelectPixel();
}

void Mapper::OnGuiRender()
{
	if (m_ImageRenderer.getCount() == 0) {
		float w = 450.f, h = 80.f;
		ImGui::SetNextWindowSize({ w, h });
		ImGui::SetNextWindowPos({ (float)conf.WIN_WIDTH / 2.f - w / 2.f, (float)conf.WIN_HEIGHT / 2.f - h / 2.f });

		ImGui::Begin("Load an image!");
		ImGui::Text("Load an Image (jpg,png,bmp,tga,hdr) to continue.\nmax resolution: 7.680 x 7.680");

		if (ImGui::Button("Select and Load")) {
			nfdchar_t* outPath = NULL;
			nfdresult_t result = NFD_OpenDialog("jpg,png,bmp,tga,hdr", NULL, &outPath);

			if (result == NFD_OKAY) {
				std::memcpy(m_WorkingPath, outPath, std::strlen(outPath));
				loadImage(outPath);
				free(outPath);
			}
			else if (result == NFD_CANCEL) {
				LOGC("Canceled selection.", LOG_COLOR::WARNING);
			}
			else {
				printf("Error: %s\n", NFD_GetError());
			}
		}

		ImGui::SameLine(0.f, 20.f);
		ImGui::Checkbox("Flip", &m_Flipped);

		ImGui::End();
	}
	else {
		ImGui::SetNextWindowSize({ (float)conf.WIN_WIDTH / 3.5f, (float)conf.WIN_HEIGHT });
		ImGui::SetNextWindowBgAlpha(0.7f);
		ImGui::SetNextWindowPos({ (float)conf.WIN_WIDTH - (float)conf.WIN_WIDTH / 3.5f, 0.f });

		ImGui::Begin("Attributes");
		if (ImGui::CollapsingHeader("Image", ImGuiTreeNodeFlags_DefaultOpen)) {
			m_LastFlippedState = m_Flipped;
			ImGui::Checkbox("Flip Uvs", &m_Flipped);
			if (m_Flipped != m_LastFlippedState && m_Flipped) {
				m_OperationImage.FlipUvs = true;
				m_ImageRenderer.RemoveAllSprites();
				m_OperationImage.Id = m_ImageRenderer.AddSprite(m_OperationImage, false);
			}
			else if (m_Flipped != m_LastFlippedState && !m_Flipped) {
				m_OperationImage.FlipUvs = false;
				m_ImageRenderer.RemoveAllSprites();
				m_OperationImage.Id = m_ImageRenderer.AddSprite(m_OperationImage, false);
			}
		}

		if (ImGui::CollapsingHeader("Reload and Discard", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Text("Save image as PNG: ");
			ImGui::SameLine(0.f, 100.f);

			static nfdchar_t* outPath = NULL;
			static nfdresult_t result{};
			if (ImGui::Button("Get Path")) {
				result = NFD_OpenDialog("jpg,png,bmp,tga,hdr", NULL, &outPath);

				if (result == NFD_CANCEL) {
					LOGC("Canceled selection.", LOG_COLOR::WARNING);
				}
				else {
					printf("Error: %s\n", NFD_GetError());
				}
			}
			ImGui::Text(outPath == NULL || *outPath == '_' || std::strlen(outPath) <= 0 ? "No path selected!" : outPath);

			static std::string msg = "";

			if (ImGui::Button("Save", { (float)conf.WIN_WIDTH / 3.5f - 20.f, 25.f })) {
				if (result == NFD_OKAY && outPath != NULL) {
					saveImage(outPath);
					free(outPath);
					outPath = new char('_');

					msg = "File Saved successfully";
				}
				else {
					msg = "This is not a valid path!";
				}
			}
			ImGui::Text(msg.c_str());

			ImGui::Separator();
			if (ImGui::Button("Discard current session")) {
				DiscardImage();
			}
		}
		ImGui::End();
	}
}

void Mapper::OnInput(GLFWwindow* window)
{
	if (m_ImageOpen) {
		glfwGetCursorPos(window, &s_MouseX, &s_MouseY);
		glfwSetScrollCallback(window, OnScrollCallback);
		ProcessMouse(window);
	}
}

void Mapper::drawSelectPixel()
{
	if (m_ImageOpen) {
		glm::vec2 imgPixel{};
		glm::vec2 mouse = { s_MouseX, conf.WIN_HEIGHT - s_MouseY };

		mouse -= CENTER * BG_TILE_WIDTH * m_Zoom + m_ViewOffset * m_Zoom;
		imgPixel = { std::floor(mouse.x / m_Zoom), m_ImageSize.y - std::ceil(mouse.y / m_Zoom) };
		if (imgPixel.x < m_ImageSize.x && imgPixel.y < m_ImageSize.y && imgPixel.x >= 0.f && imgPixel.y >= 0.f) {
			glm::vec4 rgb = m_ImageRenderer.getLastImageRGB(imgPixel);
			m_PixelRenderer.Empty();

			if (rgb.a > 0.1f) {
				constexpr float boarderFac = 1.1f;
				float borderwidth = (SELECT_PXL_SIZE - SELECT_PXL_SIZE * boarderFac) / 2.f;
				m_PixelRenderer.AddQuad({ CENTER * BG_TILE_WIDTH + imgPixel.x - SELECT_PXL_SIZE / 4.f + borderwidth,  (CENTER * BG_TILE_WIDTH + m_ImageSize.y - imgPixel.y) - SELECT_PXL_SIZE / 4.f - 1.f + borderwidth }, { SELECT_PXL_SIZE * boarderFac, SELECT_PXL_SIZE * boarderFac }, { 0.f, 0.f, 0.f, 0.5f });
			}

			m_PixelRenderer.AddQuad({ CENTER * BG_TILE_WIDTH + imgPixel.x - SELECT_PXL_SIZE / 4.f,  (CENTER * BG_TILE_WIDTH + m_ImageSize.y - imgPixel.y) - SELECT_PXL_SIZE / 4.f - 1.f }, { SELECT_PXL_SIZE, SELECT_PXL_SIZE }, rgb);

			m_PixelRenderer.Draw();
		}
	}
}

void Mapper::DiscardImage()
{
	m_ImageRenderer.RemoveAllSprites();
	m_GridRenderer.vb->Empty();

	m_Zoom = 0.f;
	m_ViewOffset = { 0.f, 0.f };
	m_ImageOpen = false;
}

void Mapper::loadImage(const std::string& path)
{
	m_OperationImage = Helper::Sprite(path, { 0.f, 0.f }, { 0.f, 0.f }, m_Flipped);
	Helper::ImageInformation information = m_OperationImage.getImageInformation();

	if (information.Size.x <= 7680 && information.Size.y <= 7680) {
		m_OperationImage.Size.x = information.Size.x;
		m_OperationImage.Size.y = information.Size.y;

		m_OperationImage.Position.x = CENTER * BG_TILE_WIDTH;
		m_OperationImage.Position.y = BG_TILE_WIDTH * CENTER;

		m_OperationImage.Id = m_ImageRenderer.AddSprite(m_OperationImage);

		m_DynamicViewBorder = information.Size + glm::ivec2(100.f, 100.f);

		m_ImageBorderRenderer.AddLine({ CENTER * BG_TILE_WIDTH + information.Size.x, CENTER * BG_TILE_WIDTH }, { CENTER * BG_TILE_WIDTH + information.Size.x, CENTER * BG_TILE_WIDTH + information.Size.y }, { 1.f, 1.f, 1.f, 0.5f });
		m_ImageBorderRenderer.AddLine({ CENTER * BG_TILE_WIDTH, CENTER * BG_TILE_WIDTH + information.Size.y }, { CENTER * BG_TILE_WIDTH + information.Size.x, CENTER * BG_TILE_WIDTH + information.Size.y }, { 1.f, 1.f, 1.f, 0.5f });

		m_ImageBorderRenderer.AddLine({ CENTER * BG_TILE_WIDTH, CENTER * BG_TILE_WIDTH }, { CENTER * BG_TILE_WIDTH + information.Size.x, CENTER * BG_TILE_WIDTH }, { 1.f, 1.f, 1.f, 0.5f });
		m_ImageBorderRenderer.AddLine({ CENTER * BG_TILE_WIDTH, CENTER * BG_TILE_WIDTH }, { CENTER * BG_TILE_WIDTH, CENTER * BG_TILE_WIDTH + information.Size.y }, { 1.f, 1.f, 1.f, 0.5f });

		m_ImageBorderRenderer.AddLine({ CENTER * BG_TILE_WIDTH + information.Size.x - 10.f, CENTER * BG_TILE_WIDTH + information.Size.y }, { CENTER * BG_TILE_WIDTH + information.Size.x + 10.f, CENTER * BG_TILE_WIDTH + information.Size.y }, { 0.f, 0.9f, 1.f, 0.8f });
		m_ImageBorderRenderer.AddLine({ CENTER * BG_TILE_WIDTH + information.Size.x, CENTER * BG_TILE_WIDTH + information.Size.y - 10.f }, { CENTER * BG_TILE_WIDTH + information.Size.x, CENTER * BG_TILE_WIDTH + information.Size.y + 10.f }, { 1.f, 0.0f, 0.8f, 0.8f });

		m_ImageSize = information.Size;
		m_ImageOpen = true;
	}
}

void Mapper::addGuideLines()
{
	m_LineRenderer.AddLine({ -1000.f, CENTER * BG_TILE_WIDTH }, { 10000.f, CENTER * BG_TILE_WIDTH }, { 1.f, 0.1f, 0.1f, 1.f });
	m_LineRenderer.AddLine({ CENTER * BG_TILE_WIDTH, -1000.f }, { CENTER * BG_TILE_WIDTH, 10000.f }, { 0.2f, 0.2f, 1.f, 1.f });
}

void Mapper::saveImage(const std::string& path)
{
	m_ImageRenderer.SaveAsFile(path.substr(0, path.size() - 4) + "_exported.png", 0);
}

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

	float x = xoffset * (1.f / m_Zoom) * 0.75;
	float y = yoffset * (1.f / m_Zoom) * 0.75;

	if (m_InTranslationMode) {
		if (m_ViewOffset.x + x < DIST_TO_CANVAS && m_ViewOffset.x + x > -m_DynamicViewBorder.x) {
			m_ViewOffset.x += x;
		}
		else if (m_ViewOffset.x + x >= DIST_TO_CANVAS) {
			m_ViewOffset.x += DIST_TO_CANVAS - m_ViewOffset.x;
		}
		else {
			m_ViewOffset.x += -m_DynamicViewBorder.x - m_ViewOffset.x;
		}

		if (m_ViewOffset.y + y < DIST_TO_CANVAS && m_ViewOffset.y + y > -m_DynamicViewBorder.y) {
			m_ViewOffset.y += y;
		}
		else if (m_ViewOffset.y + y >= DIST_TO_CANVAS) {
			m_ViewOffset.y += DIST_TO_CANVAS - m_ViewOffset.y;
		}
		else {
			m_ViewOffset.y += -m_DynamicViewBorder.y - m_ViewOffset.y;
		}
	}
}

void Mapper::OnMouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	s_MouseX = float(xpos);
	s_MouseY = float(ypos);
}

void Mapper::OnScrollCallback(GLFWwindow* window, double xpos, double ypos)
{
	float speed = m_Zoom * ZOOM_SPEED;
	if (ypos < 0.f) m_Zoom -= speed;
	else if (ypos > 0.f) m_Zoom += speed;

	m_Zoom = std::max(0.4f, m_Zoom);
	m_Zoom = std::min(ZOOM_MAX_IN, m_Zoom);
}