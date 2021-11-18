#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <SDL.h>
#include "Math.h"

// 방향광을 위한 구조체 정의
struct DirectionalLight
{
	Vector3 mDirection;
	Vector3 mDiffuseColor;
	Vector3 mSpecColor;
};

// 점광을 위한 구조체 정의
struct PointLight
{
	Vector3 mPosition;
	Vector3 mDiffuseColor;
	Vector3 mSpecColor;
	float mSpecPower;
	float mRadius;
};

class Renderer
{
public:
	Renderer(class Game* game);
	~Renderer();

	// 렌더러를 초기화하고 종료하는 함수
	bool Initialize(float screenWidth, float screenHeight);
	void Shutdown();

	void Update(float deltaTime);

	// 텍스처와 메시를 언로드
	void UnloadData();

	// 프레임을 그린다
	void Draw();

	// 스프라이트 컴포넌트 추가/삭제
	void AddSprite(class SpriteComponent* sprite);
	void RemoveSprite(class SpriteComponent* sprite);

	// 메시 컴포넌트 추가/삭제
	void AddMeshComponent(class MeshComponent* mesh);
	void RemoveMeshComponent(class MeshComponent* mesh);

	// 파일 이름을 받아서, 해당 텍스처가 맵에 존재하는지 확인. 있으면 텍스처 포인터 리턴, 없으면 파일로부터 텍스처를 로드하는 코드를 실행한다
	class Texture* GetTexture(const std::string& fileName);
	class Mesh* GetMesh(const std::string& fileName);

	/* Unprojection */
	// 화면 공간 좌표로부터 세계 공간 좌표로 변환하는 계산
	// fps에서 조준점을 기준으로 총을 쏘는데, 조준점은 화면공간 좌표. 올바르게 쏘기 위해선 이를 세계 공간 좌표로 바꿀 필요가 있다
	Vector3 Unproject(const Vector3& screenPoint) const;

	// Unproject()를 통해, 세계 공간상의 위치 계산을 하고, 
	// "화면 공간 점으로 향하는 벡터"를 만들어두면 다른 유용한 기능을 활용할 수 있다.
	// ex) picking : 3D 세계상에서 오브젝트를 클릭해 선택할 수 있는 기능
	void GetScreenDirection(Vector3& outStart, Vector3& outDir) const;

	// 각종 Get,Set
	void SetAmbientColor(Vector3 ambientColor) { mAmbientLight = ambientColor; }
	//Vector3 GetAmbientColor() { return mAmbientLight; }
	//void SetDirctionalLight(Vector3 dir, Vector3 diffuse, Vector3 specular) { mDirLight.mDiffuseColor = diffuse; mDirLight.mDirection = dir; mDirLight.mSpecColor = specular; }
	DirectionalLight& GetDirectionalLight() { return mDirLight; }
	void SetViewMatrix(Matrix4 view) { mView = view; }
	PointLight& GetPointLight(int index) { return mPointLight[index]; }

	float GetScreenWidth() const { return mScreenWidth; }
	float GetScreenHeight() const { return mScreenHeight; }

private:
	// 셰이더 파일을 로드하고 셰이더를 활성화시킨다
	bool LoadShaders();

	// VertexArray 클래스의 인스턴스를 할당하고, mSpriteVerts라는 멤버변수에 해당 인스턴스를 저장한다
	void CreateSpriteVerts();

	// 빛 처리에 필요한 uniform 값들을 전달하는 함수
	void SetLightUniforms(class Shader* shader);

	// Shader 이름으로 셰이더 포인터 찾기
	class Shader* FindShader(const std::string name);


	// 텍스처를 저장하는 맵
	/*
	* 이미지 데이터는 여러 액터가 공유해서 사용하므로 싱글톤 클래스에 가까운 Game에 파일 이름과 SDL_Texture 포인터를 쌍으로 한 맵을 만들어두자
	* 텍스처의 이름을 인자로 받고 해당 이름과 일치하는 SDL_Texture를 반환하는 GetTexture함수를 이용해서 효율적으로 로드한 텍스처를 사용하자
	*/
	// Map of textures loaded
	std::unordered_map<std::string, class Texture*> mTextures;
	// Map of meshes loaded
	std::unordered_map<std::string, class Mesh*> mMeshes;

	// All the sprite components drawn
	std::vector<class SpriteComponent*> mSprites;

	// All mesh components drawn
	std::vector<class MeshComponent*> mMeshComponents;
	std::vector<class MeshComponent*> mMeshCompPhong;
	std::vector<class MeshComponent*> mMeshCompBasic;

	// Game
	class Game* mGame;

	// Sprite shader
	class Shader* mSpriteShader;
	// Sprite vertex array
	class VertexArray* mSpriteVerts;

	// Mesh shader
	class Shader* mMeshShader;
	std::unordered_map<std::string, class Shader*> mMeshShaders;

	// View/projection for 3D shaders
	Matrix4 mView;
	Matrix4 mProjection;
	// Width/height of screen
	float mScreenWidth;
	float mScreenHeight;

	float mTimer;

	// Lighting data
	Vector3 mAmbientLight;
	DirectionalLight mDirLight;
	PointLight mPointLight[4];

	// Window
	SDL_Window* mWindow;
	// OpenGL context
	SDL_GLContext mContext;
};

