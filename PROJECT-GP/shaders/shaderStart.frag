#version 410 core

in vec3 fNormal;
in vec4 fPosEye;
in vec2 fTexCoords;
in vec4 fragPosLightSpace;
in vec3 fPosition;

out vec4 fColor;

//lighting
uniform	vec3 lightDir;
uniform	vec3 lightColor;
uniform mat4 view;

//texture
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

//shadow
uniform sampler2D shadowMap;

//control 
uniform bool enablePointLight;
uniform bool enableFog;
uniform bool muzzleFlashActive;
uniform	vec3 shellCurrentPosition;
uniform bool tankLeftActive;
uniform bool tankRightActive;

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 32.0f;

float constant = 1.0f;
float linear = 0.09f;
float quadratic = 0.032f;

void computeDirLight()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(fNormal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDir);
	
	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fPosEye.xyz);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light	
	vec3 halfVector = normalize(lightDirN + viewDirN);	
	float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
}

vec3 computePointLight(vec3 lightPosEye)
{
    vec3 lightColor = vec3(0.9f, 0.35f, 0.0f); // Orange color

    // Transform normal to eye coordinates
    vec3 normalEye = normalize(fNormal);

    // Compute light direction
    vec3 lightDirN = normalize(lightPosEye - fPosEye.xyz);

    // Compute view direction 
    vec3 viewDirN = normalize(-fPosEye.xyz); // Camera is at origin in eye coordinates

    // Compute specular light	
    vec3 halfVector = normalize(lightDirN + viewDirN);
    float specCoeff = pow(max(dot(normalEye, halfVector), 0.0f), shininess);
    vec3 specular = specularStrength * specCoeff * lightColor;

    // Compute distance to light
    float dist = length(lightPosEye - fPosEye.xyz);

    // Compute attenuation
    float att = 1.0f / (constant + linear * dist + quadratic * (dist * dist));

    // Compute ambient, diffuse, and specular components
    vec3 ambient = att * ambientStrength * lightColor;
    vec3 diffuse = att * max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	return (ambient + diffuse + specular);		
}


float computeShadow()
{
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    normalizedCoords = normalizedCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;
    float currentDepth = normalizedCoords.z;
    float bias = max(0.05f * (1.0f - dot(fNormal, lightDir)), 0.005f);
	float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

	if (normalizedCoords.z > 1.0f)
		return 0.0f;

	return shadow;
}

float computeFog()
{
 float fogDensity = 0.05f;
 float fragmentDistance = length(fPosEye);
 float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));

 return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
	computeDirLight();		
	
	ambient *= texture(diffuseTexture, fTexCoords).rgb;
	diffuse *= texture(diffuseTexture, fTexCoords).rgb;
	specular *= texture(specularTexture, fTexCoords).rgb;

	float shadow = computeShadow();

	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);


    if(enablePointLight) {
		// Convert light position to eye coordinates
		vec3 lightPosWorld = vec3(99.1813f, 7.98537, 50.4199f);
		vec3 lightPosEye = vec3(view * vec4(lightPosWorld, 1.0));
        vec3 pointLightRes = computePointLight(lightPosEye);
        color += pointLightRes;        
    }

	if(muzzleFlashActive){
		vec3 lightPosWorld = shellCurrentPosition;
		vec3 lightPosEye = vec3(view * vec4(lightPosWorld, 1.0));
		vec3 pointLightRes = computePointLight(lightPosEye);
        color += pointLightRes;        
	}

	if(tankLeftActive){
		vec3 lightPosWorld = vec3(75.1879f, 0, 54.4977f);
		vec3 lightPosEye = vec3(view * vec4(lightPosWorld, 1.0));
		vec3 pointLightRes = computePointLight(lightPosEye);
        color += pointLightRes;        
	}

	if(tankRightActive){
		vec3 lightPosWorld = vec3(83.0938f, 0, 72.6003f);
		vec3 lightPosEye = vec3(view * vec4(lightPosWorld, 1.0));
		vec3 pointLightRes = computePointLight(lightPosEye);
        color += pointLightRes;        
	}

	float fogFactor = computeFog();
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

	if(enableFog){
		fColor = mix(fogColor, vec4(color, 1.0f), fogFactor);
	}
	else {
		fColor = vec4(color, 1.0f);	
	}
}
