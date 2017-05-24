#include "ImGuiScene.h"

#include <iostream>

#include <imgui.h>

#include "ImguiGlfw3Helper.h"

ImGuiScene::ImGuiScene()
{
}

ImGuiScene::~ImGuiScene()
{
}

void ImGuiScene::initializeSceneContent()
{
	std::cout << "(gui) Initialize ImGui..." << "\n";
	ImGuiGlfw3Init(m_window, false);

	m_glfwMouseButtonFun = ImGuiGlfw3MouseButtonCallback;
	m_glfwScrollFun = ImGuiGlfw3ScrollCallback;
	m_glfwKeyFun = ImGuiGlfw3KeyCallback;
	m_glfwCharFun = ImGuiGlfw3CharCallback;
}

void ImGuiScene::cleanupSceneContent()
{
	std::cout << "(gui) Cleaning up ImGui..." << "\n";
	ImGuiGlfw3Shutdown();
}

void ImGuiScene::renderSceneContent()
{
	glfwPollEvents();
	ImGuiGlfw3NewFrame();

	// 1. Show a simple window
	// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	/*
	{
		static float f = 0.0f;
		ImGui::Text("Hello, world!");
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	*/

	ImGui::ShowTestWindow();
	ImGui::Render();
}
