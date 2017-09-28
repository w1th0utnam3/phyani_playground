#pragma once

// ImGui GLFW binding with OpenGL3 + shaders
// In this binding, ImTextureID is used to store an OpenGL 'GLuint' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

#include <imgui.h>
struct GLFWwindow;

IMGUI_API bool ImGuiGlfw3Init(GLFWwindow* window, bool install_callbacks);
IMGUI_API void ImGuiGlfw3Shutdown();
IMGUI_API void ImGuiGlfw3NewFrame();

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_API void ImGuiGlfw3InvalidateDeviceObjects();
IMGUI_API bool ImGuiGlfw3CreateDeviceObjects();

// GLFW callbacks (installed by default if you enable 'install_callbacks' during initialization)
IMGUI_API void ImGuiGlfw3MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
IMGUI_API void ImGuiGlfw3ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
IMGUI_API void ImGuiGlfw3KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
IMGUI_API void ImGuiGlfw3CharCallback(GLFWwindow* window, unsigned int c);
