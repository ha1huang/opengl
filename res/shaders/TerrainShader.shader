#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;
out vec4 FragPosInLightSpace;
out float visibility;

uniform mat4 view;
uniform mat4 projection;
uniform vec4 plane;

uniform mat4 lightView;
uniform mat4 lightSpaceMatrix;
const float density = 0.001;
const float gradient = 1.5;

void main()
{
	vec4 World_Pos = vec4(aPos, 1.0f);
	FragPos = vec3(World_Pos);
	FragPosInLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
	Normal = aNormal;
	gl_ClipDistance[0] = dot(World_Pos, plane);
	vec4 CamRelativePos = view * World_Pos;
	float CamRelativeDistance = length(CamRelativePos.xyz);
	visibility = clamp(exp(-pow((CamRelativeDistance * density), gradient)), 0.0, 1.0);
	gl_Position = projection * CamRelativePos;
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoord;
in vec3 Normal;
in vec4 FragPosInLightSpace;
in float visibility;


struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_diffuse4;
uniform sampler2D texture_diffuse5;
uniform sampler2D shadowMap;//-5
uniform vec3 skyColor;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 inputColor, float shadow)
{
	vec3 lightDir = normalize(-light.direction);
	// 漫反射着色
	float diff = max(dot(normal, lightDir), 0.0);
	// 镜面光着色
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4.0);
	// 合并结果
	vec3 ambient = light.ambient * inputColor;
	vec3 diffuse = light.diffuse * diff * inputColor;
	vec3 specular = light.specular * spec * inputColor;
	return (ambient + (diffuse + specular) * (1.0f - shadow));
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 inputColor)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 4.0);
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	// combine results
	vec3 ambient = light.ambient * inputColor;
	vec3 diffuse = 0.5 * light.diffuse * diff * inputColor;
	vec3 specular = 0.5 * light.specular * spec * inputColor;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}

float ShadowCaculation(vec4 fragPosLightSpace)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// Transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// Get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// Calculate bias (based on depth map resolution and slope)
	//float bias = max(0.005 * (1.0 - dot(Normal,normalize( -dirLight.direction))), 0.0001);
	float bias = 0.0001;
	// Check whether current frag pos is in shadow
	// float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	// Keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
	if (projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

void main()
{
	vec4 blendermapColour = texture(texture_diffuse1, TexCoord);

	float backgroundAmount = clamp(1 - (blendermapColour.r + blendermapColour.g + blendermapColour.b), 0.0, 1.0);
	vec2 tiledCoord = TexCoord * 64.0f;
	vec4 backgroundColor = texture(texture_diffuse2, tiledCoord) * backgroundAmount;
	vec4 RColor = texture(texture_diffuse3, tiledCoord) * blendermapColour.r;
	vec4 GColor = texture(texture_diffuse4, tiledCoord) * blendermapColour.g;
	vec4 BColor = texture(texture_diffuse5, tiledCoord) * blendermapColour.b;
	vec4 totalColor = backgroundColor + RColor + GColor + BColor;

	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	float shadow = ShadowCaculation(FragPosInLightSpace);
	vec3 result = CalcDirLight(dirLight, norm, viewDir, vec3(totalColor), shadow) * 1.2f;
	//for (int i = 0; i < NR_POINT_LIGHTS; i++)
	//	result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, vec3(totalColor));
	FragColor = vec4(result, 1.0);
	//FragColor = mix(vec4(skyColor, 1.0), vec4(result, 1.0), visibility);
}