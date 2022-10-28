vec3 calcPhong(vec4 lightPosition, vec3 lightColor,
			   vec3 ambientColor, vec3 diffuseColor, 
			   vec3 specularColor, float shininess) 
{
	// ambient
	vec3 ambient = ambientColor * lightColor * ambientStrength;
	
	// diffuse
	vec3 viewDir = normalize(-fragPosition);
	vec3 norm = normalize(dot(viewDir,fragNormal)<0? -fragNormal : fragNormal);
	vec3 lightDir = normalize(vec3(lightPosition)-fragPosition);
	vec3 diffuse = diffuseColor * max(dot(norm,lightDir),0.f) * lightColor;
	
	// specular
//	vec3 viewDir = normalize(-fragPosition);
	vec3 halfw = normalize(viewDir+lightDir);
	vec3 specular = specularColor * pow(max(dot(norm,halfw),0.f),shininess) * lightColor;
	
	// result
	return ambient+diffuse+specular;
}
