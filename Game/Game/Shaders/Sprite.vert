#version 330

uniform mat4 uWorldTransform;
uniform mat4 uViewProj;


// 각 버텍스에 대한 버텍스 속성을 지정해야 한다
// 이 속성은 앞서 생성했던 버텍스 배열 개체의 속성과 일치해야 하며,
// 버텍스 배열 개체는 버텍스 셰이더의 입력이 된다
// 단, GLSL의 메인 함수는 어떤 파라미터도 받지 않으므로, 셰이더 입력은 전역 변수 비슷한 형태로 받는다
// 전역 변수는 특별한 키워드로 표시된다

// 버텍스의 위치, 텍스처의 좌표라는 두 속성이 있으므로 다음과 같이 설정한다
layout(location = 0) in vec3 inPosition;    // Attrib(0)
layout(location = 1) in vec3 inNormal;    // Attrib(1)
layout(location = 2) in vec2 inTexCoord;   // Attrib(2)

// 텍스처 좌표를 fragment shader를 위해서 out 해준다
out vec2 fragTexCoord;  


void main()
{
    // 여기에 셰이더 코드를 작성

    // main이 void를 리턴하므로, 셰이더의 결과물을 저장하려면 GLSL이 제공하는 전역 변수를 사용해야 한다
    // 이번 예에서는 버텍스 위치 출력값을 저장하기 위해 기본으로 제공하는 gl_Position 변수를 사용한다
    // 지금은 버텍스 셰이더가 직접 inPosition에서 gl_Positino으로 버텍스 위치를 복사하지만,
    // gl_Position은 (x, y, z, w) 요소를 가진다. w는 이후에 알아보고, 일단은 1.0으로 설정한다
    // 그리고 지금은 버텍스 셰이더가 버텍스 속성을 수정하지 않고 단순 복사만 한다

    vec4 pos = vec4(inPosition, 1.0); // 현재 이 위치는 오브젝트 공간에 있는 것
    
    // 이 위치를 세계 변환 행렬로 곱해서 세계 공간 좌표로 변환해야 한다/ 그리고 나서 변환된 위치값을 뷰-투영 행렬로 곱해서 클립 공간의 좌표로 변환한다
    gl_Position = pos * uWorldTransform * uViewProj;

    fragTexCoord = inTexCoord;
}

