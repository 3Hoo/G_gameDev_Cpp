#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <SDL.h>
#include "Math.h"

// ���Ɽ�� ���� ����ü ����
struct DirectionalLight
{
	Vector3 mDirection;
	Vector3 mDiffuseColor;
	Vector3 mSpecColor;
};

// ������ ���� ����ü ����
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

	// �������� �ʱ�ȭ�ϰ� �����ϴ� �Լ�
	bool Initialize(float screenWidth, float screenHeight);
	void Shutdown();

	void Update(float deltaTime);

	// �ؽ�ó�� �޽ø� ��ε�
	void UnloadData();

	// �������� �׸���
	void Draw();

	// ��������Ʈ ������Ʈ �߰�/����
	void AddSprite(class SpriteComponent* sprite);
	void RemoveSprite(class SpriteComponent* sprite);

	// �޽� ������Ʈ �߰�/����
	void AddMeshComponent(class MeshComponent* mesh);
	void RemoveMeshComponent(class MeshComponent* mesh);

	// ���� �̸��� �޾Ƽ�, �ش� �ؽ�ó�� �ʿ� �����ϴ��� Ȯ��. ������ �ؽ�ó ������ ����, ������ ���Ϸκ��� �ؽ�ó�� �ε��ϴ� �ڵ带 �����Ѵ�
	class Texture* GetTexture(const std::string& fileName);
	class Mesh* GetMesh(const std::string& fileName);

	/* Unprojection */
	// ȭ�� ���� ��ǥ�κ��� ���� ���� ��ǥ�� ��ȯ�ϴ� ���
	// fps���� �������� �������� ���� ��µ�, �������� ȭ����� ��ǥ. �ùٸ��� ��� ���ؼ� �̸� ���� ���� ��ǥ�� �ٲ� �ʿ䰡 �ִ�
	Vector3 Unproject(const Vector3& screenPoint) const;

	// Unproject()�� ����, ���� �������� ��ġ ����� �ϰ�, 
	// "ȭ�� ���� ������ ���ϴ� ����"�� �����θ� �ٸ� ������ ����� Ȱ���� �� �ִ�.
	// ex) picking : 3D ����󿡼� ������Ʈ�� Ŭ���� ������ �� �ִ� ���
	void GetScreenDirection(Vector3& outStart, Vector3& outDir) const;

	// ���� Get,Set
	void SetAmbientColor(Vector3 ambientColor) { mAmbientLight = ambientColor; }
	//Vector3 GetAmbientColor() { return mAmbientLight; }
	//void SetDirctionalLight(Vector3 dir, Vector3 diffuse, Vector3 specular) { mDirLight.mDiffuseColor = diffuse; mDirLight.mDirection = dir; mDirLight.mSpecColor = specular; }
	DirectionalLight& GetDirectionalLight() { return mDirLight; }
	void SetViewMatrix(Matrix4 view) { mView = view; }
	PointLight& GetPointLight(int index) { return mPointLight[index]; }

	float GetScreenWidth() const { return mScreenWidth; }
	float GetScreenHeight() const { return mScreenHeight; }

private:
	// ���̴� ������ �ε��ϰ� ���̴��� Ȱ��ȭ��Ų��
	bool LoadShaders();

	// VertexArray Ŭ������ �ν��Ͻ��� �Ҵ��ϰ�, mSpriteVerts��� ��������� �ش� �ν��Ͻ��� �����Ѵ�
	void CreateSpriteVerts();

	// �� ó���� �ʿ��� uniform ������ �����ϴ� �Լ�
	void SetLightUniforms(class Shader* shader);

	// Shader �̸����� ���̴� ������ ã��
	class Shader* FindShader(const std::string name);


	// �ؽ�ó�� �����ϴ� ��
	/*
	* �̹��� �����ʹ� ���� ���Ͱ� �����ؼ� ����ϹǷ� �̱��� Ŭ������ ����� Game�� ���� �̸��� SDL_Texture �����͸� ������ �� ���� ��������
	* �ؽ�ó�� �̸��� ���ڷ� �ް� �ش� �̸��� ��ġ�ϴ� SDL_Texture�� ��ȯ�ϴ� GetTexture�Լ��� �̿��ؼ� ȿ�������� �ε��� �ؽ�ó�� �������
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

