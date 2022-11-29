# version 330 core

in vec3 fragNormal;
in vec3 fragPosition;
in vec2 fragTexCoords;
in vec4 lightVSPosition;

// propiedades del material
uniform sampler2D colorTexture; // ambient and diffuse components
uniform sampler2D normal0;
uniform sampler2D parallax;
uniform float height_scale;
uniform float numLayers = 1000;
uniform vec3 specularColor;
uniform float shininess;

// propiedades de la luz
uniform float ambientStrength;
uniform vec3 lightColor;
uniform bool doNormalMap;
uniform bool doSteepParallax;
uniform bool doOcclusionParallax;
in mat3 TBN;
out vec4 fragColor;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;


//#include "funcs/calcPhongNormal.frag"
vec2 ParallaxMappingSteep(vec2 texCoords, vec3 viewDir)
{ 
	// calculate the size of each layer
	float layerDepth = 1.0 / numLayers;
	// depth of current layer
	float currentLayerDepth = 0.0;
	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = viewDir.xy * height_scale; 
	vec2 deltaTexCoords = P / numLayers;
	
	vec2  currentTexCoords     = texCoords;
	float currentDepthMapValue = texture(parallax, currentTexCoords).r;
	
	while(currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(parallax, currentTexCoords).r;  
		// get depth of next layer
		currentLayerDepth += layerDepth;  
	}
	
	return currentTexCoords;  
}
vec2 ParallaxMappingOcclusion(vec2 texCoords, vec3 viewDir)
{ 

	// calculate the size of each layer
	float layerDepth = 1.0 / numLayers;
	// depth of current layer
	float currentLayerDepth = 0.0;
	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = viewDir.xy * height_scale; 
	vec2 deltaTexCoords = P / numLayers;
	
	vec2  currentTexCoords     = texCoords;
	float currentDepthMapValue = texture(parallax, currentTexCoords).r;
	
	while(currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(parallax, currentTexCoords).r;  
		// get depth of next layer
		currentLayerDepth += layerDepth;  
	}
	
	// get texture coordinates before collision (reverse operations)
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(parallax, prevTexCoords).r - currentLayerDepth + layerDepth;

	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords; 	
}
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
	float height =  texture(parallax, texCoords).r;    
	vec2 p = viewDir.xy / viewDir.z * (height * height_scale);
	return texCoords - p;    
} 
void main() {
	//NormalMap
	
	float height =  texture(parallax, fragTexCoords).r;  
	
	vec3 viewDir = vec3(normalize(TangentViewPos - TangentFragPos));
	vec2 texCoords;
	if(doOcclusionParallax){
		texCoords= ParallaxMappingOcclusion(fragTexCoords,viewDir);
	}
	else{
		if(doSteepParallax){
			texCoords= ParallaxMappingSteep(fragTexCoords,viewDir);
		}
		else{
			texCoords= ParallaxMapping(fragTexCoords,viewDir);
		}
	}
	if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0) discard;
	
	// diffuse lighting
	
	//vec3 normal = normalize(texture(normal0, texCoords).xyz* 2 -1);
	vec3 normal;
	if(doNormalMap){
	normal = normalize(texture(normal0, texCoords).xyz* 2 -1);
	}
	else{
		normal=fragNormal;
	}
	vec4 tex = texture(colorTexture,texCoords);
	vec3 ambient = 0.1 * vec3(tex);
	vec3 lightDir = normalize(vec3(TangentLightPos-TangentFragPos));
	
	float diff = max(dot(lightDir, normal), 0.0f);
	
	vec3 diffuse = vec3(tex) * diff * lightColor;
	// specular    
	vec3 reflectDir = vec3(reflect(-lightDir, normal));

	vec3 halfwayDir = vec3(normalize(lightDir + viewDir));
	float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
	
	vec3 specular = specularColor * spec;
	fragColor = vec4(ambient + diffuse + specular, 1.0);
}

