#include "SubDivMeshRenderer.hpp"


SubDivMeshRenderer::SubDivMeshRenderer (const std::vector<glm::vec3> & pos, 
										const std::vector<glm::vec3> & norms, 
										const std::vector<int> & lines,
										const std::vector<int> & tris) 
{
	glGenVertexArrays(1,&VAO);
	glBindVertexArray(VAO);
	glGenBuffers(4, XBO);
	
	glBindBuffer(GL_ARRAY_BUFFER, XBO[0]);
	glBufferData(GL_ARRAY_BUFFER, pos.size()*sizeof(glm::vec3), pos.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, XBO[1]);
	glBufferData(GL_ARRAY_BUFFER, norms.size()*sizeof(glm::vec3), norms.data(), GL_STATIC_DRAW);
	
	npoints = pos.size();
	
	if ( (nlines=lines.size())!=0 ) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, XBO[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, lines.size()*sizeof(int), lines.data(), GL_STATIC_DRAW);
	}
	
	if ( (ntris=tris.size())!=0 ) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, XBO[3]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, tris.size()*sizeof(int), tris.data(), GL_STATIC_DRAW);
	}
	
	glBindVertexArray(0);
	
}

void SubDivMeshRenderer::drawPoints(Shader &shader) const {
	if (ntris==0) return;
	glBindVertexArray(VAO);
	shader.setBuffer("vertexPosition",XBO[0],GL_FLOAT,3);
	shader.setBuffer("vertexNormal",XBO[1],GL_FLOAT,3,false);
	glPointSize(3);
	glDrawArrays(GL_POINTS,0,npoints);	
	glBindVertexArray(0);
}

void SubDivMeshRenderer::drawLines(Shader &shader) const {
	if (ntris==0) return;
	glBindVertexArray(VAO);
	shader.setBuffer("vertexPosition",XBO[0],GL_FLOAT,3);
	shader.setBuffer("vertexNormal",XBO[1],GL_FLOAT,3,false);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, XBO[2]);
	glDrawElements(GL_LINES,nlines,GL_UNSIGNED_INT,0);	
	glBindVertexArray(0);
}

void SubDivMeshRenderer::drawTriangles(Shader &shader) const {
	if (ntris==0) return;
	glBindVertexArray(VAO);
	shader.setBuffer("vertexPosition",XBO[0],GL_FLOAT,3);
	shader.setBuffer("vertexNormal",XBO[1],GL_FLOAT,3,false);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, XBO[3]);
	glPolygonOffset(1,1);
	glEnable( GL_POLYGON_OFFSET_FILL );
	glDrawElements(GL_TRIANGLES,ntris,GL_UNSIGNED_INT,0);	
	glDisable( GL_POLYGON_OFFSET_FILL );
	glBindVertexArray(0);
}

void SubDivMeshRenderer::freeResources( ) {
	if (VAO==0) return;
	glDeleteBuffers(4,XBO);
	glDeleteVertexArrays(1,&VAO);
}
SubDivMeshRenderer::~SubDivMeshRenderer ( ) {
	freeResources();
}

SubDivMeshRenderer makeRenderer(SubDivMesh & m) {
	
	const auto &e = m.e;
	const auto &n = m.n;
	
	// normales por elemento
	std::vector<glm::vec3> enorms;
	enorms.reserve(e.size());
	for(const Elemento &ei : e) {
		glm::vec3 normal = glm::normalize( glm::cross(n[ei[2]].p-n[ei[0]].p,n[ei[1]].p-n[ei[0]].p) );
		if (ei.nv==4) {
			glm::vec3 normal2 = glm::normalize( glm::cross(n[ei[3]].p-n[ei[0]].p,n[ei[2]].p-n[ei[0]].p) );
			normal = glm::normalize(normal+normal2);
		}
		enorms.push_back(-normal);
	}
	
	// normales por nodo
	std::vector<glm::vec3> vnorms, vpos;
	vnorms.reserve(n.size()); vpos.reserve(n.size());
	for(const Nodo &ni : n) {
		glm::vec3 r={};
		for(int ie : ni.e)
			r+=enorms[ie];
		if (not ni.e.empty()) 
			r = glm::normalize(r);
		vnorms.push_back(r);
		vpos.push_back(ni.p);
	}
	
	std::vector<int> lines, tris;
	for(int ie=0;ie<static_cast<int>(e.size());++ie) {
		const Elemento &ei = e[ie];
		tris.push_back(ei[0]); tris.push_back(ei[1]); tris.push_back(ei[2]);
		if (ei.v[0]<ie) { lines.push_back(ei[0]); lines.push_back(ei[1]); }
		if (ei.v[1]<ie) { lines.push_back(ei[1]); lines.push_back(ei[2]); }
		if (ei.v[2]<ie) { lines.push_back(ei[2]); lines.push_back(ei[3]); }
		if (ei.nv==3) continue;
		tris.push_back(ei[0]); tris.push_back(ei[2]); tris.push_back(ei[3]);
		if (ei.v[3]<ie) { lines.push_back(ei[3]); lines.push_back(ei[0]); }
	}
	
	return SubDivMeshRenderer(vpos,vnorms,lines,tris);
}


SubDivMeshRenderer::SubDivMeshRenderer (SubDivMeshRenderer &&o) {
	*this = static_cast<const SubDivMeshRenderer&>(o);
	o = static_cast<const SubDivMeshRenderer&>(SubDivMeshRenderer());
}

SubDivMeshRenderer &SubDivMeshRenderer::operator=(SubDivMeshRenderer &&o) {
	freeResources();
	*this = static_cast<const SubDivMeshRenderer&>(o);
	o = static_cast<const SubDivMeshRenderer&>(SubDivMeshRenderer());
	return *this;
}
