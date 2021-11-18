#version 330

// ������ ��ŸŸ�� ��� Ÿ�̸�
uniform float uTime;

// ���� ���۷� ��µǴ� �÷�
out vec4 outColor;

// vertex�κ��� �޴� �ؽ�ó ��ǥ (������ Ÿ�԰� �̸��̾�� �Ѵ�)
in vec2 fragTexCoord;



// �׸��� ���� �ؽ�ó ��ǥ�κ��� ���������� ��� ���� �ؽ�ó ���÷� uniform�� �ϳ� �����Ѵ�
uniform sampler2D uTexture;
// Ư���ϰ� ���� sampler2D�� c++���� ���ε��� �ʿ䰡 ����. ���� ���������� �� ���� ���� �ϳ��� �ؽ�ó�� ���ε��ϱ� �����̴�
// �׷��� OpenGL�� �ڵ����� ���̴��� ����ó ���÷��� Ȱ��ȭ�� �ؽ�ó�� �����ϰ� �������� �ȴ�

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
    // �ؼ��� �Ҵ��Ͽ�, �ؽ�ó�κ��� ������ ���ø� �Ѵ�
    vec4 tex = texture(uTexture, fragTexCoord); // �־��� �ؽ�ó�� normalized �� RGBA�� ����ȴ�
    vec3 overlay = vec3(fragTexCoord.xy, abs(sin(uTime)) * 10.0f);
    vec3 newColor = blendOverlay(tex.rgb, overlay);

    outColor = tex;
}