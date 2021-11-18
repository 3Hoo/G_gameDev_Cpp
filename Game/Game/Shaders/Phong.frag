#version 330

// ������ ��ŸŸ�� ��� Ÿ�̸�
uniform float uTime;

// ���� ���۷� ��µǴ� �÷�
out vec4 outColor;

// vertex�κ��� �޴� �ؽ�ó ��ǥ (������ Ÿ�԰� �̸��̾�� �Ѵ�)
in vec2 fragTexCoord;

in vec3 fragNormal;
in vec3 fragWorldPos;


// �׸��� ���� �ؽ�ó ��ǥ�κ��� ���������� ��� ���� �ؽ�ó ���÷� uniform�� �ϳ� �����Ѵ�
uniform sampler2D uTexture;
// Ư���ϰ� ���� sampler2D�� c++���� ���ε��� �ʿ䰡 ����. ���� ���������� �� ���� ���� �ϳ��� �ؽ�ó�� ���ε��ϱ� �����̴�
// �׷��� OpenGL�� �ڵ����� ���̴��� ����ó ���÷��� Ȱ��ȭ�� �ؽ�ó�� �����ϰ� �������� �ȴ�

// ���Ɽ�� ���� ����ü ����
struct DirectionalLight
{
    // ���Ɽ
    vec3 mDirection;
    // diffuse ����
    vec3 mDiffuseColor;
    // specular ����
    vec3 mSpecColor;
};

// ������ ���� ����ü ����
struct PointLight
{
	vec3 mPosition;
	vec3 mDiffuseColor;
	vec3 mSpecColor;
	float mSpecPower;
	float mRadius;
};

// ������ ���� uniform
// ���� ���������� ī�޶� ��ġ
uniform vec3 uCameraPos;
// ambient(�ֺ���)
uniform vec3 uAmbientLight;
// ǥ�鿡 ���� ���ݻ�(specular) ����
uniform float uSpecPower;
// ���Ɽ(������ ���� �ϳ�)
uniform DirectionalLight uDirLight;
// ����
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
    // ǥ�� ����
    // OpenGL�� �ﰢ���� �� ǥ�鿡 ���� �ش� �ȼ��� ���� ���͸� ��� ����, �ﰢ���� �ִ� ���ؽ��� ���� ���͵��� �����ϴµ�, 
    // ������ �� �ܰ迡�� �� ����ȭ�� ���͸� ������ ��, �� ����� ����ȭ�� ���͸� �������� �����Ƿ� ������ȭ ���ش�
    vec3 N = normalize(fragNormal);

    // ǥ�鿡�� ���������� ����
    // ���Ɽ�� �� �������� �߻��ϹǷ� ���� ���͸� ������Ű�� �ȴ�
    vec3 L = normalize(-uDirLight.mDirection);

    // ǥ�鿡�� ī�޶�� ���ϴ� ����
    vec3 V = normalize(uCameraPos - fragWorldPos);

    // N�� ���� -L�� �ݻ�
    vec3 R = normalize(reflect(-L, N));

    // Point Light�� ���� ��
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
    
    // �� �ݻ� ���
    vec3 Phong = uAmbientLight;
    float NdotL = dot(N, L);
    if (NdotL > 0)
    {
        vec3 Diffuse = (uDirLight.mDiffuseColor + PointDiffuse) * NdotL;
        vec3 Specular = (uDirLight.mSpecColor + PointSpecColor) * pow(max(0.0, dot(R, V)), uSpecPower + PointSpecPower);
        Phong += Diffuse + Specular;
    }

    // �������� �ؽ�ó ���� X Phong ���� (���İ� = 1)
    outColor = texture(uTexture, fragTexCoord) * vec4(Phong, 1.0f);
}