#version 330

uniform mat4 uWorldTransform;
uniform mat4 uViewProj;


// �� ���ؽ��� ���� ���ؽ� �Ӽ��� �����ؾ� �Ѵ�
// �� �Ӽ��� �ռ� �����ߴ� ���ؽ� �迭 ��ü�� �Ӽ��� ��ġ�ؾ� �ϸ�,
// ���ؽ� �迭 ��ü�� ���ؽ� ���̴��� �Է��� �ȴ�
// ��, GLSL�� ���� �Լ��� � �Ķ���͵� ���� �����Ƿ�, ���̴� �Է��� ���� ���� ����� ���·� �޴´�
// ���� ������ Ư���� Ű����� ǥ�õȴ�

// ���ؽ��� ��ġ, �ؽ�ó�� ��ǥ��� �� �Ӽ��� �����Ƿ� ������ ���� �����Ѵ�
layout(location = 0) in vec3 inPosition;    // Attrib(0)
layout(location = 1) in vec3 inNormal;    // Attrib(1)
layout(location = 2) in vec2 inTexCoord;   // Attrib(2)

// �ؽ�ó ��ǥ�� fragment shader�� ���ؼ� out ���ش�
out vec2 fragTexCoord;  

// vertex shader�� gl_Position�� Ŭ�� �������� ��ġ => ǥ�鿡�� ī�޶�� ���ϴ� ���� ���͸� ��������, ���� �������� ��ġ�� �ʿ�
// ���� ���������� ��ġ. ����ؼ� fragment shader���� �������ش�
out vec3 fragWorldPos;

// �Էµ� ��������(inNormal)�� ������Ʈ ������ �ִ�. �� ���� ���� �������� �ʿ��ϴ�
// ���� ���������� ����
out vec3 fragNormal;

void main()
{
    // ���⿡ ���̴� �ڵ带 �ۼ�

    // main�� void�� �����ϹǷ�, ���̴��� ������� �����Ϸ��� GLSL�� �����ϴ� ���� ������ ����ؾ� �Ѵ�
    // �̹� �������� ���ؽ� ��ġ ��°��� �����ϱ� ���� �⺻���� �����ϴ� gl_Position ������ ����Ѵ�
    // ������ ���ؽ� ���̴��� ���� inPosition���� gl_Positino���� ���ؽ� ��ġ�� ����������,
    // gl_Position�� (x, y, z, w) ��Ҹ� ������. w�� ���Ŀ� �˾ƺ���, �ϴ��� 1.0���� �����Ѵ�
    // �׸��� ������ ���ؽ� ���̴��� ���ؽ� �Ӽ��� �������� �ʰ� �ܼ� ���縸 �Ѵ�

    // ��ġ�� ���� ��ǥ�� ��ȯ
    vec4 pos = vec4(inPosition, 1.0); // ���� �� ��ġ�� ������Ʈ ������ �ִ� ��
    
    // ��ġ�� ���� �������� ��ġ�� ��ȯ
    pos = pos * uWorldTransform;

    // ���� �������� ��ġ�� ����
    fragWorldPos = pos.xyz;

    // ��ġ�� Ŭ�� ���� ��ǥ�� ��ȯ (��-���� ���)
    gl_Position = pos * uViewProj;

    // ������ ���� �������� �������� ��ȯ (�������� �̵��� �ǹ̰� �����Ƿ�, ���� ��ǥ w�� 0���� �Ѵ�. �̷��� �Ǹ� ���躯ȯ����� �̵� ��Ұ� �������� 0�� �ȴ�)
    fragNormal = (vec4(inNormal, 0.0f) * uWorldTransform).xyz;

    // �ؽ�ó ��ǥ�� fragment shader���� ����
    fragTexCoord = inTexCoord;
}
