#include "ScreenCapture.hpp"
#include "Debug.hpp"

ScreenCapture::ScreenCapture() : shader("shaders/capture") { 
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(2, VBO);
	
	loc_pos = glGetAttribLocation(shader.getProgramId(), "vertexPosition"); 
	cg_assert(loc_pos!=-1,"Shader does not have vertexPosition attribute");
	glEnableVertexAttribArray(loc_pos);
	loc_tc = glGetAttribLocation(shader.getProgramId(), "vertexTexCoords"); 
	cg_assert(loc_tc!=-1,"Shader does not have vertexTexCoords attribute");
	glEnableVertexAttribArray(loc_tc);
	
	verts[0]=verts[1]=verts[2]=verts[3]={0.f,0.f,0.f};
	tex_coords[0]={0.f,0.f};
	tex_coords[1]={1.f,0.f};
	tex_coords[2]={0.f,1.f};
	tex_coords[3]={1.f,1.f};
}

ScreenCapture::~ScreenCapture() {
	if (tex_id) glDeleteTextures(1,&tex_id);
	glDeleteBuffers(2,VBO);
	glDeleteVertexArrays(1,&VAO);
}

void ScreenCapture::draw() {
	glBindVertexArray(VAO);
	
	shader.use();
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), verts, GL_DYNAMIC_DRAW); 
	glVertexAttribPointer(loc_pos, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(loc_pos);
	
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), tex_coords, GL_DYNAMIC_DRAW);  
	glVertexAttribPointer(loc_tc, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(loc_tc);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_id);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glDrawArrays(GL_TRIANGLE_STRIP, 0,4);
	glBindTexture(GL_TEXTURE_2D, 0); 
	
	glBindVertexArray(0);
}


void ScreenCapture::take (int x, int y, int w, int h, int factor) {
	glBindVertexArray(VAO);
	
	glReadBuffer(GL_BACK);
	glActiveTexture(GL_TEXTURE0);
	if (tex_id==0) {
		glGenTextures(1,&tex_id);
		glBindTexture(GL_TEXTURE_2D, tex_id);
		glTexImage2D    (GL_TEXTURE_2D, 0, GL_RGBA,     w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	} else {
		glBindTexture(GL_TEXTURE_2D, tex_id);
	}
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x,y,w,h, 0);
	glBindTexture(GL_TEXTURE_2D, 0); 
	
	glBindVertexArray(0);
	
	verts[0].x=0;
	verts[1].x=verts[3].x=w*factor;
	verts[2].y=verts[3].y=h*factor;
}


Shader & ScreenCapture::getShader ( ) {
	shader.use();
	return shader;
}

