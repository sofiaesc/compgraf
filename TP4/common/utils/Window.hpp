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
	
	enum Flags { fNone=0, fImGui=1, fAntialiasing=2, fBlend=4, fDepth=8 };
	static int fDefaults; // = fImGui|fAntialiasing|fDepth;
	
	Window() = default;
	Window(int w, int h, const std::string &title, int flags=fDefaults, GLFWwindow *share_context_with=nullptr);
	~Window();
	
	Window(const Window &other) = delete;
	Window &operator=(const Window &other) = delete;
	
	Window(Window &&other);
	Window &operator=(Window &&other);
	
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
