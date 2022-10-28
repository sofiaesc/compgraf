vec3 calcPhong(vec3 fragNormal,
			   vec4 lightPosition, vec3 lightColor,
			   vec3 ambientColor, vec3 diffuseColor, 
			   vec3 specularColor, float shininess) 
{
	// ambient
	vec3 ambient = ambientColor * lightColor * ambientStrength;
	
	// diffuse
	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(vec3(lightPosition)/*-fragPosition*/);
	vec3 diffuse = diffuseColor * max(dot(norm,lightDir),0.f) * lightColor;
	
	// specular
	vec3 viewDir = normalize(-fragPosition);
	vec3 reflectDir = reflect(-lightDir,norm);
	vec3 specular = specularColor * pow(max(dot(viewDir,reflectDir),0.f),shininess) * lightColor;
	
	// result
	return ambient+diffuse+specular;
}
