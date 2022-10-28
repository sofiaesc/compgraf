#ifndef WINDOWs_HPP
#define WINDOW_HPP

#include <vector>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <functional>

class Window {
public:
	Window() = default;
	Window(int w, int h, const std::string &title, bool enable_imgui=false, GLFWwindow *share_context_with=nullptr);
	~Window();
	
	Window(const Window &w) = delete;
	Window &operator=(const Window &w) = delete;
	
	void ImGuiDialog(const char *title, const std::function<void()> &func);
	void ImGuiFrame(const std::function<void()> &func);
	operator bool() const { return win_ptr!=nullptr; }
	operator GLFWwindow*() { return win_ptr; }
	operator const GLFWwindow*() const { return win_ptr; }
private:
	static int windows_count;
	GLFWwindow *win_ptr = nullptr;
	ImGuiContext *imgui_context = nullptr;
};

class FrameTimer {
public:
	FrameTimer();
	double newFrame();
	int getFrameRate() const;
private:
	double prev, fps_t;
	int fps = 0, fps_aux=0;
};

namespace ImGui {
	bool Combo(const char *label, int *current_item, const std::vector<std::string> &items);
};

#endif
