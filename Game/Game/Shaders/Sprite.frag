#version 330

// 게임의 델타타임 기반 타이머
uniform float uTime;

// 색상 버퍼로 출력되는 컬러
out vec4 outColor;

// vertex로부터 받는 텍스처 좌표 (동일한 타입과 이름이어야 한다)
in vec2 fragTexCoord;



// 그리고 얻은 텍스처 좌표로부터 색상정보를 얻기 위해 텍스처 샘플러 uniform을 하나 설정한다
uniform sampler2D uTexture;
// 특이하게 현재 sampler2D는 c++에서 바인딩할 필요가 없다. 지금 구조에서는 한 번에 오직 하나의 텍스처만 바인딩하기 때문이다
// 그래서 OpenGL은 자동으로 셰이더의 텕스처 샘플러가 활성화된 텍스처에 유일하게 대응함을 안다

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

void main()
{
    // 텍셀을 할당하여, 텍스처로부터 색상을 샘플링 한다
    vec4 tex = texture(uTexture, fragTexCoord); // 주어진 텍스처의 normalized 된 RGBA가 저장된다
    vec3 overlay = vec3(fragTexCoord.xy, abs(sin(uTime)) * 10.0f);
    vec3 newColor = blendOverlay(tex.rgb, overlay);

    outColor = tex;
}