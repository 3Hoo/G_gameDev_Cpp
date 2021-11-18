#include "Renderer.h"
#include "Game.h"
#include "Texture.h"
#include "Mesh.h"
#include "VertexArray.h"
#include "Shader.h"
#include "SpriteComponent.h"
#include "MeshComponent.h"
#include "AudioSystem.h"
#include "UIScreen.h"
#include <string>
#include <GL/glew.h>


Renderer::Renderer(Game* game)
	: mGame(game)
	, mSpriteShader(nullptr)
	, mMeshShader(nullptr)
	, mTimer(0)
{

}

Renderer::~Renderer()
{

}


bool Renderer::Initialize(float screenWidth, float screenHeight)
{
	mScreenHeight = screenHeight;
	mScreenWidth = screenWidth;


	// �ʱ�ȭ�� ���������� SDL_CreateWindow�� ȭ���� ��� ���ʴ�

	// OpenGl ������ ���� ������, OpenGL�� ����, ���� ���� ���� �Ķ���� �Ӽ� ������ ���� SDL_GL_SetAttribute�� ȣ���Ѵ�
	// ȣ���� �����ϸ� 0��ȯ, �����ϸ� ���� ��ȯ
	// �ھ�, ȣȯ��, ES �� ����ũž�� ���� ������ �ھ� OpenGL �������� ���
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// 3.3�������� ����
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// RGBA ä�θ��� 8��Ʈ ũ���� ���� ���� ��û
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	// ���� ���۸� Ȱ��ȭ
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// OpenGL�� �ϵ���� ������ ����ϵ��� ����
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	// Context�� ����� ����, ZBuffer(Depth Buffer)�� Ȱ��ȭ��Ű��(���� ũ�⸦ 24��Ʈ�� ��´�)
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// ������ ����, �������� x,y ��ǥ(ȭ�� ������� ����), �������� ���̿� ����, ������ �������� �÷���
	mWindow = SDL_CreateWindow("Game Programming in C++ (CH1)", 100, 100, 
		static_cast<int>(mScreenWidth), static_cast<int>(mScreenHeight), SDL_WINDOW_OPENGL);
	if (!mWindow)
	{
		SDL_Log("Failed to create window : %s", SDL_GetError());
		return false;
	}
	else
	{
		SDL_Log("TEST # Window Created");
	}


	// ���� OpenGL�� context�� �����. 
	// context�� OpenGL�� �ν��ϴ� ��� ����, ������Ʈ�� �����ϴ� OpenGL�� ������ �����ϸ� �ȴ� (���� ����, �ε�� �̹����� ��, �پ��� OpenGL ������Ʈ���� �����Ѵ�)
	// ������ �ϳ��� OpenGL���α׷��� �ټ��� context�� ������ �� ������, �� å������ �ϳ��� ����Ѵ�
	mContext = SDL_GL_CreateContext(mWindow);	// �� �ڵ�� OpenGL context�� �����쿡 �����Ѵ�


	// OpenGL�� Ȯ�� �ý��۰��� ���� ȣȯ���� ����������, ���ϴ� Ȯ�� ����� �������� ��û�ؾ� �Ѵ�
	// �̸� ����ȭ�ϱ� ���� GLEW ���� �ҽ� ���̺귯���� ����Ѵ�
	// GLEW�� �ڵ������� ���� OpenGL Context �������� �����ϴ� ��� Ȯ�� �Լ��� �ʱ�ȭ�Ѵ�
	// GLEW �ʱ�ȭ
	glewExperimental = GL_TRUE;	// �Ϻ� �÷������� �ھ� context�� ����� �� �߻��� ���� �ִ� �ʱ�ȭ ������ �����Ѵ�
	if (glewInit() != GLEW_OK)
	{
		SDL_Log("Failed to initialize GLEW");
		return false;
	}
	else
	{
		SDL_Log("TEST # glew Initialized");
	}
	// �Ϻ� �÷������� GLEW�� ���� �ڵ带 �������Ƿ�, �������� ��������
	glGetError();

	// ���̴��� �ε��Ѵ�
	if (!LoadShaders())
	{
		SDL_Log("Failed to load shaders.");
		return false;
	}
	else
	{
		SDL_Log("TEST # Shaders Loaded");
	}

	// Crete quad for drawing sprites
	CreateSpriteVerts();

	return true;
	/*
	// Renderer �ʱ�ȭ
	mRenderer = SDL_CreateRenderer(mWindow,		// �������� ���� ������
								   -1,			// � �׷��� ī�带 �������. ������ ���� �����츦 ����� �� �ǹ̰� ����� ��. �Ϲ������� -1(�⺻ ��ī ���)
								   SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC  // �ʱ�ȭ �÷���. ����ȭ ������(�׷��� �ϵ���� Ȱ��) |  ��������ȭ Ȱ��ȭ  => �� �Լ��� ������ �÷�����
	);
	if (mRenderer == nullptr)
	{
		SDL_Log("Failed to create Renderer : %s", SDL_GetError());
		return false;
	}
	*/

	/*
	// 2D �׷���(�̹���)�� ���� SDL_Image �ʱ�ȭ
	if (IMG_Init(IMG_INIT_PNG) == 0) // PNG�� ����� ����. BMP�� SDL�� SDL_image ���̵� �����ϱ� ������ bmp ���Ÿ� �ʿ� ����. 0�̸� �ʱ�ȭ ����
	{
		SDL_Log("Failed to initialize SDL_Image : %s", SDL_GetError());
		return false;
	}
	*/
}

void Renderer::Shutdown()
{
	delete mSpriteVerts;

	mSpriteShader->Unload();
	delete mSpriteShader;

	mMeshShader->Unload();
	delete mMeshShader;

	for (auto iter = mMeshShaders.begin(); iter != mMeshShaders.end(); iter++)
	{
		iter->second->Unload();
		delete iter->second;
	}
	

	// gl context ����
	SDL_GL_DeleteContext(mContext);

	// SDL_DestroyWindow�� SDL_Window ��ü�� �ı�
	SDL_DestroyWindow(mWindow);
}

void Renderer::Update(float deltaTime)
{
	mTimer += deltaTime;
	mSpriteShader->SetFloatUniform("uTime", mTimer);	

	/*
	if (!mMeshComponents.empty())
	{
		for (auto iter = mMeshComponents.begin() ; iter!=mMeshComponents.end() ; iter++)
		{
			std::string shader = (*iter)->GetShaderName();
			if (shader == "Phong")
			{
				mMeshCompPhong.emplace_back(*iter);
			}
			else if (shader == "BasicMesh")
			{
				mMeshCompBasic.emplace_back(*iter);
			}
			else
				continue;
		}
	}
	*/
}


void Renderer::UnloadData()
{
	// textures destroy
	for (auto j : mTextures)
	{
		j.second->UnLoad();
		delete j.second;
	}
	mTextures.clear();

	// destroy meshes
	for (auto j : mMeshes)
	{
		j.second->Unload();
		delete j.second;
	}
	mMeshes.clear();
}


void Renderer::Draw()
{
	/* 1
	* �׷��Ƚ������� Color Buffer�� ȭ�� ��� �ȼ��� ���� ������ �����صд� (x,y) �迭
	* ���� ������ "��� ����" �ܰ� ���� �� �����ӿ��� ������ �׷��� ����� "Color Buffer"�� ����
	* 24��Ʈ Color Depth = RGB 8��Ʈ��. ���⿡ ���� 8��Ʈ => �� 32��Ʈ.  1080p Color Buffer = 1920x1080x4byte = 7.9MB
	* "Frame Buffer"�� ��Ȯ�� ���� = ���� ���� + ��Ÿ ����(���� ���� / ���ٽ� ����) �� ����
	*/

	//-----------------------------------------------------------------------------------------------------------------------------

	/* 2
	* ������ �����Ӽӵ��� ȭ���� ������ ���� �ӵ��� �ٸ� �� �߻��ϴ� Screen Tearing�� �ذ��Ϸ��� 2������ ������ �ʿ��ϴ�
	* 1. ���Ӱ� ���÷��̰� �����ϴ� ���� ���۸� 2����(�ĸ�, ����) �ΰ�, ����� ����Ѵ� => Double Buffering
	*		=> ���� ó���� ������ �ĸ� ���۸� ����Ѵ�
	* 2. ���� X���۸� ���÷��̰� ���� �ִµ� ������ Y���ۿ� �� ���� �ٷ� X�� ����Ϸ��� �Ѵٸ�? => ������ ���÷��̰� ����X�� �� �׸� ������ ��ٷ��� �Ѵ�
	*		=> �̸� "���� ����ȭ" / "Vertical Synchronization" / "vsync" �� �Ѵ�
	*
	* ����� �ֱ� ���÷��̴� Adaptive Refresh Rate(������ ȭ�� ��� ��) ��� ����� ���.
	* ���÷��̰� ȭ���� ������ �� ���ӿ� �����ϴ� ���� �ƴ϶�, ������ ���÷��̿��� ���� ȭ���� �������� �˷��ְ� �ϴ� ���
	*/

	//-----------------------------------------------------------------------------------------------------------------------------

	/* 3
	* �⺻������ ȭ���� �׸��� ���� ���� �ܰ�� �̷������ (�� �����Ӹ��� �۵�)
	* 1. �ĸ� ���۸� �ܻ����� Ŭ�����Ѵ� (������ ���� ����) => �� �� ZBuffer�� ���鵵 1.0���� �ʱ�ȭ���ش�
	* 2. ��ü ���� ����� �׸���
	* 3. ���� ���ۿ� �ĸ� ���۸� ��ȯ�Ѵ�
	*/

	// ������ ȸ������ ����  1
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// ���� ����, ZBuffer => (Frame Buffer) �ʱ�ȭ
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// GL_COLOR_BUFFER_BIT�� �� �Լ��� ȣ���ϸ�, ���� ���۸� ������ �������� ä���

	// ����� �׸���  2

	// 3D �������� ���� ���� �� ���� ��ü�� ����� ������ �ϱ� ����(������ ���� ���� ������ ���� ���� �� ũ�� ������, ������ �κ��� �׷����� �ʴ´�)
	// ȭ�� �˰����� �̿��Ѵ� 
	// (������ ���� �׸���, ������ �ڿ������� ������ ������. �ȼ��� ������ �� ������ �������� ������Ʈ �ʸӿ� �ִ� ������ �ȼ��� �׷����� �ʵ��� �� �ȼ��� ���̸� �׽�Ʈ)
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	mMeshShader->SetActive();
	mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);
	SetLightUniforms(mMeshShader);

	/*
	for (auto iter = mMeshShaders.begin(); iter != mMeshShaders.end(); iter++)
	{
		iter->second->SetActive();
		// ��-���� ��� ����
		iter->second->SetMatrixUniform("uViewProj", mView * mProjection);
		// ����Ʈ ����
		SetLightUniforms(iter->second);
	}
	*/

	for (auto mc : mMeshComponents)
	{
		mc->Draw(mMeshShader);
	}

	/*
	for (auto mc : mMeshCompPhong)
	{
		mc->Draw(FindShader(mc->GetShaderName()));
	}

	for (auto mc : mMeshCompBasic)
	{
		mc->Draw(FindShader(mc->GetShaderName()));
	}
	*/

	//---------------------------------------------------------------------------

	// ���� ������ ���ؼ� �ɼ��� Ų�� => ��������Ʈ ���� ���� (�̷��� �ϸ� 2D ��������Ʈ�� 3D ����� ���� �տ� ��Ÿ���� => �̸� ���� UI, HUD�� �����Ű��)
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(
		GL_SRC_ALPHA,	// srcFactor = srcAlpha
		GL_ONE_MINUS_SRC_ALPHA	// dstFactor = 1 - srcAlpha
	);
	// ��������Ʈ ���̴��� ���ؽ� �迭 ��ü�� Ȱ��ȭ
	mSpriteShader->SetActive();
	mSpriteVerts->SetActive();
	// ��� ��������Ʈ���� �׸���
	for (auto sprite : mSprites)
	{
		sprite->Draw(mSpriteShader);
	}

	// UIScreen�� Texture�� ����ϹǷ�, ��������Ʈ�� ����ϴ� ���� ���̴��� �̿��ؼ� UI�� �׸���'
	for (auto ui : mGame->GetUIStack())
	{
		ui->Draw(mSpriteShader);
	}

	// ���۸� �����ؼ� ����� ����Ѵ�  3
	SDL_GL_SwapWindow(mWindow);		// ���� ���ۿ� �ĸ� ���۸� ��ü�Ѵ�

	//SDL_SetRenderDrawColor(mRenderer, 220, 220, 220, 255); // �ĸ� ���۸� ����� ���� �ϳ��� �� ����
	//SDL_RenderClear(mRenderer); // 1
	// ��� ��������Ʈ���� �׸���
	//for (auto sprite : mSprites)	// 2
	//{
	//	sprite->Draw(mRenderer);
	//}
	//SDL_RenderPresent(mRenderer); // 3
}

void Renderer::AddSprite(SpriteComponent* sprite)
{
	int order = sprite->GetDrawOrder();
	auto iter = mSprites.begin();

	for (; iter != mSprites.end(); ++iter)
	{
		if (order < (*iter)->GetDrawOrder())
		{
			break;
		}
	}

	mSprites.insert(iter, sprite);
}

void Renderer::RemoveSprite(SpriteComponent* sprite)
{
	auto iter = std::find(mSprites.begin(), mSprites.end(), sprite);
	mSprites.erase(iter);
}

void Renderer::AddMeshComponent(MeshComponent* mesh)
{
	//const std::string& shaderName = mesh->GetMesh()->GetShaderName();	// -> �� �� �� ������ ��� �ڵ尡 �۵��� ����� ���� �ʴ´�. ����?
	//SDL_Log("%s", shaderName.c_str());

	/*
		const std::string& shaderName = mesh->GetMesh()->GetShaderName();
	SDL_Log("%s",shaderName.c_str());
	auto iter = mMeshShaders.find(shaderName);
	SDL_Log("Shader program does not have appropriate shader for the mesh");
	if (iter == mMeshShaders.end())
	{
		SDL_Log("Shader program does not have appropriate shader for the mesh");
		return;
	}

	if (iter->first == "Phong")
	{
		mMeshCompPhong.emplace_back(mesh);
		SDL_Log("phongmc++");
	}
	else if (iter->first == "BasicMesh")
	{
		mMeshCompBasic.emplace_back(mesh);
		SDL_Log("basicmc++");
	}
	else
	{
		SDL_Log("Shader program does not have appropriate shader for the mesh");
		return;
	}*/

	mMeshComponents.emplace_back(mesh);
}

void Renderer::RemoveMeshComponent(MeshComponent* mesh)
{
	auto iter = std::find(mMeshComponents.begin(), mMeshComponents.end(), mesh);
	mMeshComponents.erase(iter);

	/*
	iter = std::find(mMeshCompPhong.begin(), mMeshCompPhong.end(), mesh);
	mMeshCompPhong.erase(iter);

	iter = std::find(mMeshCompBasic.begin(), mMeshCompBasic.end(), mesh);
	mMeshCompBasic.erase(iter);
	*/
}

Texture* Renderer::GetTexture(const std::string& fileName)
{
	/*
	// ���Ϸκ��� SDL_Surface ������ �̹��� �ε�
	// �����ϸ� SDL_Surface �����͸� ����, �����ϸ� nullptr
	SDL_Surface* IMG_Load(
		const char* file // �̹��� ���� �̸�
		)

	// SDL_CreateTexutreFromSurface�� SDL_Surface�� SDL�� ȭ�鿡 �׸��� �� �ʿ��� ������ SDL_Texture�� ��ȯ���ش�
	// �����ϸ� SDL_Texture ������ ��ȯ, �����ϸ� nullptr
	SDL_Texture* SDL_CreateTextureFromSurface(
	SDL_Renderer * renderer,	// ����� ������
	SDL_Surface * surface	// ��ȯ�� SDL_Surface
		)
	*/

	Texture* text = nullptr;

	auto itr = mTextures.find(fileName);

	if (itr != mTextures.end())
	{
		text = itr->second;
	}
	else
	{

		text = new Texture();
		if (text->Load(fileName))
		{
			mTextures.emplace(fileName, text);
		}
		else
		{
			delete text;
			text = nullptr;
		}
	}

	return text;
}

Mesh* Renderer::GetMesh(const std::string& fileName)
{
	Mesh* mesh = nullptr;
	auto iter = mMeshes.find(fileName);
	if (iter != mMeshes.end())
	{
		mesh = iter->second;
	}
	else
	{
		mesh = new Mesh();
		if (mesh->Load(fileName, this))
		{
			mMeshes.emplace(fileName, mesh);
		}
		else
		{
			delete mesh;
			mesh = nullptr;
		}
	}
	return mesh;
}

Vector3 Renderer::Unproject(const Vector3& screenPoint) const
{
	// ���� ȭ�������ǥ x,y�� [-1,1] �������� ������ ��ġ ��ǥ�� ����ȭ��Ų��
	Vector3 deviceCoord = screenPoint;
	deviceCoord.x /= mScreenWidth * 0.5f;	// OpenGL�� �߾��� (0,0) ������ ���� (512,384) �̹Ƿ� ����/���� ������ �������� ������ ����� ����ȭ�� �ȴ�
	deviceCoord.y /= mScreenHeight * 0.5f;

	// �� �� �ϳ��� x,y ��ǥ�� [0,1] ������ ������ z���� ������(1�̸� �ָ� �ִ� ����� �� = ī�޶�� �� �� �ִ� �ִ� �Ÿ�)
	// �׷��Ƿ� ������������ ����� �����Ϸ���, [0,1] ������ z ��Ұ��� �ʿ�
	// normalized Device Coord = (ndcX, ndcY, z, 1)
	
	// ���������� ����� ��-���� ����� �����
	Matrix4 unprojection = mView * mProjection;
	unprojection.Invert();

	// �� ��, ���������� ����� NDC�� ���ϸ� w ��Ұ��� ����ȴ�. �׷��� �� ��ڰ��� w�� ������ w��Ҹ� 1�� �ǵ����� ������ȭ�� �ʿ��ϴ�
	// temp = (ndc)(Unprojection)
	// worldPos = temp / temp.w

	// ���������� ��İ� w��Ҹ� ����ؼ� ���� ������ȭ�� �����Ѵ�
	return Vector3::TransformWithPerspDiv(deviceCoord, unprojection);
}

void Renderer::GetScreenDirection(Vector3& outStart, Vector3& outDir) const
{
	// ���� ���͸� ��������, Unproject()�� �� �� �����Ѵ�
	// ���������� �� ��, �������� �� ��
	// �� �� ���Ͱ��� ������ �ϰ�, �� ���͸� ����ȭ�ϸ� ���� ���͸� ���� �� �ִ�

	// �������� ��´�(����� ���(z = 0)�� ȭ����� �߽�)
	Vector3 screenPoint(0.0f, 0.0f, 0.0f);
	outStart = Unproject(screenPoint);
	// �� ���� ��´�(����� ���� �� ���(z = 1) ���̿� �ִ� ȭ����� ����)
	screenPoint.z = 0.9f;
	Vector3 end = Unproject(screenPoint);

	// ���� ���͸� ��´�
	outDir = end - outStart;
	outDir.Normalize();
}

void Renderer::SetLightUniforms(Shader* shader)
{
	// ī�޶� ��ġ�� �� ����� ����ķ� ���� �� �ִ�
	Matrix4 invView = mView;
	invView.Invert();
	shader->SetVectorUniform("uCameraPos", invView.GetTranslation());	// ������� �� ��° ���� ù 3��Ұ� ī�޶��� ���� ���� ��ġ�� �ش��Ѵ�

	// �ֺ���(Ambient)
	shader->SetVectorUniform("uAmbientLight", mAmbientLight);

	// ���Ɽ
	shader->SetVectorUniform("uDirLight.mDirection", mDirLight.mDirection);
	shader->SetVectorUniform("uDirLight.mDiffuseColor", mDirLight.mDiffuseColor);
	shader->SetVectorUniform("uDirLight.mSpecColor", mDirLight.mSpecColor);

	// ����
	shader->SetVectorUniform("uPotLight[0].mPosition", mPointLight[0].mPosition);
	shader->SetVectorUniform("uPotLight[0].mDiffuseColor", mPointLight[0].mDiffuseColor);
	shader->SetVectorUniform("uPotLight[0].mSpecColor", mPointLight[0].mSpecColor);
	shader->SetFloatUniform("uPotLight[0].mSpecPower", mPointLight[0].mSpecPower);
	shader->SetFloatUniform("uPotLight[0].mRadius", mPointLight[0].mRadius);

	shader->SetVectorUniform("uPotLight[1].mPosition", mPointLight[1].mPosition);
	shader->SetVectorUniform("uPotLight[1].mDiffuseColor", mPointLight[1].mDiffuseColor);
	shader->SetVectorUniform("uPotLight[1].mSpecColor", mPointLight[1].mSpecColor);
	shader->SetFloatUniform("uPotLight[1].mSpecPower", mPointLight[1].mSpecPower);
	shader->SetFloatUniform("uPotLight[1].mRadius", mPointLight[1].mRadius);

	shader->SetVectorUniform("uPotLight[2].mPosition", mPointLight[2].mPosition);
	shader->SetVectorUniform("uPotLight[2].mDiffuseColor", mPointLight[2].mDiffuseColor);
	shader->SetVectorUniform("uPotLight[2].mSpecColor", mPointLight[2].mSpecColor);
	shader->SetFloatUniform("uPotLight[2].mSpecPower", mPointLight[2].mSpecPower);
	shader->SetFloatUniform("uPotLight[2].mRadius", mPointLight[2].mRadius);

	shader->SetVectorUniform("uPotLight[3].mPosition", mPointLight[3].mPosition);
	shader->SetVectorUniform("uPotLight[3].mDiffuseColor", mPointLight[3].mDiffuseColor);
	shader->SetVectorUniform("uPotLight[3].mSpecColor", mPointLight[3].mSpecColor);
	shader->SetFloatUniform("uPotLight[3].mSpecPower", mPointLight[3].mSpecPower);
	shader->SetFloatUniform("uPotLight[3].mRadius", mPointLight[3].mRadius);
}

bool Renderer::LoadShaders()
{
	// Sprite Shader
	mSpriteShader = new Shader();
	if (!mSpriteShader->Load("Shaders/Sprite.vert", "Shaders/Sprite.frag"))
	{
		return false;
	}
	else
	{
		SDL_Log("TEST # Sprite Shader Loaded");
	}

	mSpriteShader->SetActive();

	// uniform ������ ���� ��������
	// ��-���� ����� ���α׷����� ������� �����Ƿ� �� ���� ���� => �׷��� ���⿡�� ������
	// ��������Ʈ ��ǻ��Ʈ�� ������Ʈ ������ ������ ���� ��ȯ ��ķ� �׷����Ƿ�, ���� ��ȯ ����� �����Ӹ��� �����ؾ� �Ѵ� => spritecomponent�� draw���� �����Ѵ�
	Matrix4 viewProj = Matrix4::CreateSimpleViewProj(mScreenWidth, mScreenHeight);
	mSpriteShader->SetMatrixUniform("uViewProj", viewProj);


	// Mesh Shader
	mMeshShader = new Shader();
	if (!mMeshShader->Load("Shaders/Phong.vert", "Shaders/Phong.frag"))
	{
		return false;
	}
	else
	{
		SDL_Log("TEST # Mesh Shader Loaded");
	}
	mMeshShader->SetActive();


	Shader* temp = new Shader();
	if (!temp->Load("Shaders/Phong.vert", "Shaders/Phong.frag"))
	{
		return false;
	}
	temp->SetActive();	// ������ ��� �޽ð� ���� ���̴��� ����Ѵٰ� ����. �� �޽ø��� ���� �ٸ� ���̴��� ����� ���� �ִ�
	mMeshShaders.emplace("Phong", temp);

	temp = new Shader();
	if (!temp->Load("Shaders/BasicMesh.vert", "Shaders/BasicMesh.frag"))
	{
		return false;
	}
	temp->SetActive();
	mMeshShaders.emplace("BasicMesh", temp);
	
	// ��, ���� ��� ����
	mView = Matrix4::CreateLookAt(
		Vector3::Zero,	// ī�޶� ��ġ
		Vector3::UnitX,	// ī�޶� �ٶ󺸴� ����
		Vector3::UnitZ	// ī�޶��� ���� ����
	);
	mProjection = Matrix4::CreatePerspectiveFOV(
		Math::ToRadians(70.0f),	// ����  FOV
		mScreenWidth,			// ���� �ʺ�
		mScreenHeight,			// ���� ����
		25.0f,					// ����� ������ �Ÿ�
		10000.0f				// �� ������ �Ÿ�
	);
	mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);
	for (auto iter = mMeshShaders.begin(); iter != mMeshShaders.end(); ++iter)
	{
		iter->second->SetMatrixUniform("uViewProj", mView * mProjection);
	}

	//mGame->GetAudioSystem()->SetListener(mView);

	return true;
}

void Renderer::CreateSpriteVerts()
{
	float vertices[] = {
		-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f, // top left
		0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, // top right
		0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, // bottom right
		-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f  // bottom left
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}

Shader* Renderer::FindShader(const std::string name)
{
	auto iter = mMeshShaders.find(name);

	if (iter != mMeshShaders.end())
	{
		return iter->second;
	}
	else
	{
		SDL_Log("Can't find Shader : %s", name);
		return nullptr;
	}
}

