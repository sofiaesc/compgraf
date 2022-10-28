#include <stdexcept>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "Window.hpp"
#include "Debug.hpp"
#include <iomanip>
#include <sstream>


namespace ImGui {
	bool Combo(const char *label, int *current_item, const std::vector<std::string> &items) {
		return ImGui::Combo(label, current_item, 
							[](void* data, int idx, const char** out_text) { 
								*out_text = (*reinterpret_cast<const std::vector<std::string>*>(data))[idx].c_str(); return true; 
							}, 
							(void*)&items, items.size(), -1);
	}
}

int Window::windows_count = 0;

Window::Window (int w, int h, const std::string & title, bool enable_imgui, GLFWwindow *share_context_with) {
	if (windows_count==0) {
		glfwSetErrorCallback([](int code, const char *message){ 
			std::stringstream scode; scode<<"0x"<<std::hex<<code;
			cg_error("GLFW code "+scode.str()+": "+message); 
		}); 
		if (not glfwInit()) cg_error("Failed to initialize GLFW");
	}
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE); // mac-os bug?
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES,4); // antialiasing
	
	win_ptr = glfwCreateWindow(w,h,title.c_str(),nullptr,share_context_with);
	cg_assert(win_ptr,"Failed to create GLFW window");
	glfwMakeContextCurrent(win_ptr);
	
	if (windows_count==0 and (not gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)))
		cg_error("Failed to initialize GLAD")
		
	cg_info(std::string("OpenGL version: ")+reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	glfwSetInputMode(win_ptr,GLFW_STICKY_KEYS,GL_TRUE);
	glfwSetFramebufferSizeCallback(win_ptr,[](GLFWwindow *, int w, int h) { glViewport(0,0,w,h); } );
	
	if (enable_imgui) {
		IMGUI_CHECKVERSION();
		imgui_context = ImGui::CreateContext();
		ImGui::SetCurrentContext(imgui_context); // si no es el 1er context, el create no lo define como current
		ImGui_ImplGlfw_InitForOpenGL(win_ptr,true);
		ImGui_ImplOpenGL3_Init("#version 330");
		ImGui::StyleColorsDark();
	}
	
	++windows_count;
}

Window::~Window ( ) {
	if (imgui_context) {
		ImGui::SetCurrentContext(imgui_context);
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	glfwDestroyWindow(win_ptr);
	if (--windows_count==0)
		glfwTerminate();
}

void Window::ImGuiDialog (const char * title, const std::function<void()> & func) {
	cg_assert(imgui_context,"ImGui not initialized for this window");
	ImGui::SetCurrentContext(imgui_context);
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	if (title) ImGui::Begin(title);
	func();
	if (title) ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::ImGuiFrame (const std::function<void()> & func) {
	ImGuiDialog(nullptr,func);
}

FrameTimer::FrameTimer() {
	prev = fps_t = glfwGetTime();
}

double FrameTimer::newFrame() {
	double cur = glfwGetTime();
	double delta = cur-prev;
	prev  = cur;
	++fps_aux;
	while (cur-fps_t>1.0) {
		fps = fps_aux;
		fps_aux = 0;
		fps_t += 1.0;
	}
	return delta;
}

int FrameTimer::getFrameRate() const { 
	return fps; 
}


