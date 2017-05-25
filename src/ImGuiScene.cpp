#include "ImGuiScene.h"

#include <iostream>
#include <limits>

#include <imgui.h>

#include "ImguiGlfw3Helper.h"
#include "Simulation.h"
#include "AnimationLoop.h"
#include "DemoScene.h"

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

	// TODO: Add glfwSetCharModsCallback callback

	m_glfwMouseButtonFun = [](GLFWwindow* window, int button, int action, int mods) -> bool {
		ImGuiGlfw3MouseButtonCallback(window, button, action, mods);
		return ImGui::IsMouseHoveringAnyWindow();
	};

	m_glfwScrollFun = [](GLFWwindow* window, double xoffset, double yoffset) -> bool {
		ImGuiGlfw3ScrollCallback(window, xoffset, yoffset);
		return ImGui::IsMouseHoveringAnyWindow();
	};

	m_glfwKeyFun = [](GLFWwindow* window, int key, int scancode, int action, int mods) -> bool {
		ImGuiGlfw3KeyCallback(window, key, scancode, action, mods);
		return true;
	};

	m_glfwCharFun = [](GLFWwindow* window, unsigned int c) -> bool {
		ImGuiGlfw3CharCallback(window, c);
		return true;
	};
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

	drawMainWindow();

	ImGui::ShowTestWindow();
	ImGui::Render();
}

void ImGuiScene::drawMainWindow()
{
	if (!ImGui::Begin("Physical animation playground", &m_options.mainUiOpen)) {
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::PushItemWidth(-140);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Spacing();

	if (ImGui::CollapsingHeader("Camera")) {
		const auto rotation = m_camera->rotation();

		if (ImGui::Button("Reset camera")) m_camera->resetToDefault();
		ImGui::Text("Zoom: %.4e", m_camera->zoom());
		ImGui::Text("Rotation: w=%.4e,x=%.4e,y=%.4e,z=%.4e", rotation.w, rotation.x, rotation.y, rotation.z);
	}

	if (ImGui::CollapsingHeader("Animation")) {
		AnimationLoop& animationLoop = Simulation::getAnimationLoop();

		if (ImGui::Button("Reset scene")) Simulation::getAnimationScene().resetScene();

		ImGui::Text("Automatic timestepping");
		if (ImGui::Checkbox("Start/stop time", &m_options.automaticTimestepping) || ImGui::IsKeyPressed(GLFW_KEY_SPACE) ) animationLoop.toggleAutomaticTimestepping(m_options.timeStretch);
		ImGui::SliderFloat("Time stretch factor", &m_options.timeStretch, 0.0f + std::numeric_limits<float>::epsilon(), 100, "%.5f", 10);

		ImGui::Text("Manual timestepping");
		if (ImGui::Button("Increment timestep")) animationLoop.requestTimestep(m_options.timestep);
		ImGui::SliderFloat("Manual timestep size", &m_options.timestep, 0.0f + std::numeric_limits<float>::epsilon(), 100, "%.5f", 10);
	}

	ImGui::End();
}
