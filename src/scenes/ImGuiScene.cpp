#include "ImGuiScene.h"

#include <iostream>
#include <limits>

#include <imgui.h>

#include "ImGuiGlfw3Helper.h"
#include "MathHelper.h"

#include "Simulation.h"
#include "AnimationLoop.h"

ImGuiScene::ImGuiScene()
{
}

ImGuiScene::~ImGuiScene()
{
}

void ImGuiScene::initializeSceneContent()
{
	std::cout << "(gui) Initialize ImGui..." << "\n";
	ImGui_ImplGlfwGL3_Init(m_window, false);

	// TODO: Add glfwSetCharModsCallback callback

	m_glfwMouseButtonFun = [](GLFWwindow* window, int button, int action, int mods) -> bool {
		ImGui_ImplGlfwGL3_MouseButtonCallback(window, button, action, mods);
		return (ImGui::IsMouseHoveringAnyWindow() || ImGuizmo::IsOver() || ImGuizmo::IsUsing());
	};

	m_glfwScrollFun = [](GLFWwindow* window, double xoffset, double yoffset) -> bool {
		ImGui_ImplGlfwGL3_ScrollCallback(window, xoffset, yoffset);
		return ImGui::IsMouseHoveringAnyWindow();
	};

	m_glfwKeyFun = [](GLFWwindow* window, int key, int scancode, int action, int mods) -> bool {
		ImGui_ImplGlfwGL3_KeyCallback(window, key, scancode, action, mods);
		return true;
	};

	m_glfwCharFun = [](GLFWwindow* window, unsigned int c) -> bool {
		ImGui_ImplGlfwGL3_CharCallback(window, c);
		return true;
	};
}

void ImGuiScene::cleanupSceneContent()
{
	std::cout << "(gui) Cleaning up ImGui..." << "\n";
	ImGui_ImplGlfwGL3_Shutdown();
}

void ImGuiScene::renderSceneContent()
{
	glfwPollEvents();
	ImGui_ImplGlfwGL3_NewFrame();

	ImGuizmo::BeginFrame();
	ImGuizmo::Enable(m_options.gizmoEnabled);
    if (m_options.gizmoEnabled)
        editTransform(m_options.tempTransform);

	drawMainWindow();

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

	// FPS counting
	ImGui::Text("Render average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	static double timestepTime, dt;
	std::tie(timestepTime, dt) = Simulation::getAnimationLoop().lastTimestepStats();
	ImGui::Text("Animation %.3e ms/timestep, dt=%.3e", timestepTime*1000, dt*1000);
	ImGui::Spacing();

	// Camera settings
	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
		const auto rotation = m_camera->rotation();

		if (ImGui::Button("Reset camera")) m_camera->resetToDefault();
		ImGui::Text("Scaling:\n %.4e", m_camera->scaling().x);
		ImGui::Text("Rotation:\n w=% 11.4e\n x=% 11.4e\n y=% 11.4e\n z=% 11.4e", rotation.w, rotation.x, rotation.y, rotation.z);

		ImGui::Checkbox("Gizmo enabled", &m_options.gizmoEnabled);
		ImGui::Text("Gizmo mode:");
		ImGui::SameLine();
		if (ImGui::RadioButton("Translate", m_options.currentGizmoOperation == ImGuizmo::TRANSLATE))
			m_options.currentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", m_options.currentGizmoOperation == ImGuizmo::ROTATE))
			m_options.currentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", m_options.currentGizmoOperation == ImGuizmo::SCALE))
			m_options.currentGizmoOperation = ImGuizmo::SCALE;
	}

	// Simulation, timestepping settings
	if (ImGui::CollapsingHeader("Animation")) {
		AnimationLoop& animationLoop = Simulation::getAnimationLoop();

		//if (ImGui::Button("Reset scene")) Simulation::getAnimationScene().resetScene();

		ImGui::Text("Automatic timestepping");
		if (ImGui::Checkbox("Start/stop time", &m_options.automaticTimestepping)
			|| ImGui::IsKeyPressed(GLFW_KEY_SPACE))
			animationLoop.toggleAutomaticTimestepping(m_options.timeStretch);
		ImGui::SliderFloat("Time stretch factor",
						   &m_options.timeStretch, 0.0f + std::numeric_limits<float>::epsilon(), 100, "%.5f", 10);

		ImGui::Text("Manual timestepping");
		if (ImGui::Button("Increment timestep")) animationLoop.requestTimestep(m_options.timestep);
		ImGui::SliderFloat("Manual timestep size",
						   &m_options.timestep, 0.0f + std::numeric_limits<float>::epsilon(), 100, "%.5f", 10);
	}

	ImGui::End();
}

void ImGuiScene::editTransform(glm::fmat4& matrix)
{
	//static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	//static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

	if (ImGui::IsKeyPressed(GLFW_KEY_1))
		m_options.currentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(GLFW_KEY_2))
		m_options.currentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(GLFW_KEY_3))
		m_options.currentGizmoOperation = ImGuizmo::SCALE;

	/*
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

	//std::cout << std::boolalpha << "over: " << ImGuizmo::IsOver() << ", using: " << ImGuizmo::IsUsing() << std::endl;
	
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	const glm::dquat rotation = m_camera->rotation();

	glm::dmat4 view = glm::fmat4(1.0f);
	view = glm::translate(view, -m_camera->translation());
	//view = glm::scale(view, m_camera->scaling());
	view = glm::rotate(view, glm::angle(rotation), glm::axis(rotation));

	const glm::fmat4 projection = m_camera->projectionMatrix();

	if (!m_options.gizmoPreviouslyInUse) {
		switch (m_options.currentGizmoOperation) {
		case ImGuizmo::ROTATE:
			matrix = glm::dmat4(m_camera->rotation());
			break;
		}
	}

	const bool gizmoInUse = ImGuizmo::IsUsing();
	const bool gizmoUseEnded = m_options.gizmoPreviouslyInUse && !gizmoInUse;
	m_options.gizmoPreviouslyInUse = gizmoInUse;

	glm::fmat4 fview(view);
	ImGuizmo::Manipulate(glm::value_ptr(fview), glm::value_ptr(projection),
			m_options.currentGizmoOperation, ImGuizmo::LOCAL, glm::value_ptr(matrix));

	if (gizmoUseEnded) {
		switch (m_options.currentGizmoOperation) {
		case ImGuizmo::ROTATE:
			glm::dquat rot = glm::dquat(matrix);
			m_camera->setRotation(rot);
			break;
		}
	}
}
