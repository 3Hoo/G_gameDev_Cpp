#version 330

// 게임의 델타타임 기반 타이머
uniform float uTime;

// 색상 버퍼로 출력되는 컬러
out vec4 outColor;

// vertex로부터 받는 텍스처 좌표 (동일한 타입과 이름이어야 한다)
in vec2 fragTexCoord;

in vec3 fragNormal;
in vec3 fragWorldPos;


// 그리고 얻은 텍스처 좌표로부터 색상정보를 얻기 위해 텍스처 샘플러 uniform을 하나 설정한다
uniform sampler2D uTexture;
// 특이하게 현재 sampler2D는 c++에서 바인딩할 필요가 없다. 지금 구조에서는 한 번에 오직 하나의 텍스처만 바인딩하기 때문이다
// 그래서 OpenGL은 자동으로 셰이더의 텕스처 샘플러가 활성화된 텍스처에 유일하게 대응함을 안다

// 방향광을 위한 구조체 정의
struct DirectionalLight
{
    // 방향광
    vec3 mDirection;
    // diffuse 색상
    vec3 mDiffuseColor;
    // specular 색상
    vec3 mSpecColor;
};

// 점광을 위한 구조체 정의
struct PointLight
{
	vec3 mPosition;
	vec3 mDiffuseColor;
	vec3 mSpecColor;
	float mSpecPower;
	float mRadius;
};

// 조명을 위한 uniform
// 세계 공간에서의 카메라 위치
uniform vec3 uCameraPos;
// ambient(주변광)
uniform vec3 uAmbientLight;
// 표면에 대한 정반사(specular) 지수
uniform float uSpecPower;
// 방향광(지금은 오직 하나)
uniform DirectionalLight uDirLight;
// 점광
uniform PointLight uPotLight[4];


vec3 blendOverlay(vec3 background, vec3 overlay)
{
    vec3 returnColor;

    if(background.x > 0.5f)
    {
        float value_unit = (1.0f - background.x) / 0.25f;
        float min_value = background.x - (1.0f - overlay.x);
        returnColor.x = (overlay.x * value_unit) + min_value;
    }
    else
    {
        float value_unit = background.x / 0.25f;
        returnColor.x = overlay.x * value_unit;
    }

    if(background.y > 0.5f)
    {
        float value_unit = (1.0f - background.y) / 0.25f;
        float min_value = background.y - (1.0f - overlay.y);
        returnColor.y = (overlay.y * value_unit) + min_value;
    }
    else
    {
        float value_unit = background.y / 0.25f;
        returnColor.y = overlay.y * value_unit;
    }

    if(background.z > 0.5f)
    {
        float value_unit = (1.0f - background.z) / 0.25f;
        float min_value = background.z - (1.0f - overlay.z);
        returnColor.z = (overlay.z * value_unit) + min_value;
    }
    else
    {
        float value_unit = background.z / 0.25f;
        returnColor.z = overlay.z * value_unit;
    }

    return returnColor;
}

float PosLength(vec3 pointPos, vec3 objectPos)
{
    return sqrt(pow(pointPos.x - objectPos.x, 2) + pow(pointPos.y - objectPos.y, 2) + pow(pointPos.z - objectPos.z, 2));
}

void main()
{
    // 표면 법선
    // OpenGL이 삼각형의 전 표면에 걸쳐 해당 픽셀의 법선 벡터를 얻기 위해, 삼각형에 있는 버텍스의 법선 벡터들을 보간하는데, 
    // 보간의 각 단계에서 두 정규화된 벡터를 보간할 시, 그 결과는 정규화된 벡터를 보장하지 않으므로 재정규화 해준다
    vec3 N = normalize(fragNormal);

    // 표면에서 광원으로의 벡터
    // 방향광은 한 방향으로 발산하므로 광원 벡터를 반전시키면 된다
    vec3 L = normalize(-uDirLight.mDirection);

    // 표면에서 카메라로 향하는 벡터
    vec3 V = normalize(uCameraPos - fragWorldPos);

    // N에 대한 -L의 반사
    vec3 R = normalize(reflect(-L, N));

    // Point Light을 위한 색
    vec3 PointDiffuse;
    vec3 PointSpecColor;
    float PointSpecPower;
    float attenuation;
    float lightPower = 4.0f;
    //1.0	0.022	0.0019

    for(int i = 0 ; i < 4 ; i++)
    {
        vec3 ppos = uPotLight[i].mPosition;
        float distance = PosLength(ppos, fragWorldPos);
        if(distance <= uPotLight[i].mRadius)
        {
            attenuation = 1.0f / (1.0f + (0.022 * distance) + (0.0019 * distance));
            PointDiffuse += uPotLight[i].mDiffuseColor * attenuation * lightPower;
            PointSpecColor += uPotLight[i].mSpecColor * attenuation * lightPower;
            PointSpecPower += uPotLight[i].mSpecPower * attenuation;
        }
    }
    //PointSpecPower /= 4.0f;
    
    // 퐁 반사 계산
    vec3 Phong = uAmbientLight;
    float NdotL = dot(N, L);
    if (NdotL > 0)
    {
        vec3 Diffuse = (uDirLight.mDiffuseColor + PointDiffuse) * NdotL;
        vec3 Specular = (uDirLight.mSpecColor + PointSpecColor) * pow(max(0.0, dot(R, V)), uSpecPower + PointSpecPower);
        Phong += Diffuse + Specular;
    }

    // 최종색은 텍스처 색상 X Phong 광원 (알파값 = 1)
    outColor = texture(uTexture, fragTexCoord) * vec4(Phong, 1.0f);
}