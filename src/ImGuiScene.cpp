#include "ImGuiScene.h"

#include <iostream>
#include <limits>

#include <imgui.h>

#include "ImGuiGlfw3Helper.h"
#include "ImGuizmo.h"
#include "Simulation.h"
#include "AnimationLoop.h"
#include "DemoScene.h"
#include "MathHelper.h"

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

	ImGuizmo::BeginFrame();
	ImGuizmo::Enable(true);
	editTransform(m_options.tempTransform);

	drawMainWindow();

	//ImGui::ShowTestWindow();
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

	ImGui::Text("Render average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	static double timestepTime, dt;
	std::tie(timestepTime, dt) = Simulation::getAnimationLoop().lastTimestepStats();
	ImGui::Text("Animation %.3e ms/timestep, dt=%.3e", timestepTime*1000, dt*1000);
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

void ImGuiScene::editTransform(glm::fmat4& matrix)
{
	/*
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	if (ImGui::IsKeyPressed(90))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(&matrix[0][0], matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation, 3);
	ImGui::InputFloat3("Rt", matrixRotation, 3);
	ImGui::InputFloat3("Sc", matrixScale, 3);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, &matrix[0][0]);

	if (mCurrentGizmoOperation != ImGuizmo::SCALE) {
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}
	
	static bool useSnap(false);
	if (ImGui::IsKeyPressed(83))
		useSnap = !useSnap;
	ImGui::Checkbox("", &useSnap);
	ImGui::SameLine();
	glm::fvec3 snap;
	switch (mCurrentGizmoOperation) {
	case ImGuizmo::TRANSLATE:
		ImGui::InputFloat3("Snap", &snap.x);
		break;
	case ImGuizmo::ROTATE:
		ImGui::InputFloat("Angle Snap", &snap.x);
		break;
	case ImGuizmo::SCALE:
		ImGui::InputFloat("Scale Snap", &snap.x);
		break;
	}
	*/
	
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	static glm::fmat4 view = m_camera->modelViewMatrix();
	static glm::fmat4 projection = m_camera->projectionMatrix();
	ImGuizmo::DrawCube(&view[0][0], &projection[0][0], &matrix[0][0]);
	ImGuizmo::Manipulate(&view[0][0], &projection[0][0], ImGuizmo::ROTATE, ImGuizmo::WORLD, &matrix[0][0]);
}
