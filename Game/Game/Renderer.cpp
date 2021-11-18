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


	// 초기화에 성공했으면 SDL_CreateWindow로 화면을 띄울 차례다

	// OpenGl 윈도우 생성 이전에, OpenGL의 버전, 색상 깊이 등의 파라미터 속성 설정을 위해 SDL_GL_SetAttribute를 호출한다
	// 호출이 성공하면 0반환, 실패하면 음수 반환
	// 코어, 호환성, ES 중 데스크탑에 가장 적합한 코어 OpenGL 프로파일 사용
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// 3.3버전으로 지정
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	// RGBA 채널마다 8비트 크기인 색상 버퍼 요청
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	// 더블 버퍼링 활성화
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	// OpenGL이 하드웨서 가속을 사용하도록 강제
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	// Context를 만들기 전에, ZBuffer(Depth Buffer)를 활성화시키자(보통 크기를 24비트로 잡는다)
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// 윈도우 제목, 윈도우의 x,y 좌표(화면 좌측상단 기준), 윈도우의 높이와 넓이, 윈도우 생성관련 플래그
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


	// 이제 OpenGL의 context를 만든다. 
	// context는 OpenGL이 인식하는 모든 상태, 오브젝트를 포함하는 OpenGL의 세계라고 생각하면 된다 (색상 깊이, 로드된 이미지나 모델, 다양한 OpenGL 오브젝트들을 포함한다)
	// 본래는 하나의 OpenGL프로그램에 다수의 context를 생성할 수 있지만, 이 책에서는 하나만 사용한다
	mContext = SDL_GL_CreateContext(mWindow);	// 이 코드로 OpenGL context를 윈도우에 생성한다


	// OpenGL은 확장 시스템과의 하위 호환성을 지원하지만, 원하는 확장 기능을 수동으로 요청해야 한다
	// 이를 간소화하기 위해 GLEW 오픈 소스 라이브러리를 사용한다
	// GLEW는 자동적으로 현재 OpenGL Context 버전에서 지원하는 모든 확장 함수를 초기화한다
	// GLEW 초기화
	glewExperimental = GL_TRUE;	// 일부 플랫폼에서 코어 context를 사용할 때 발생할 수도 있는 초기화 에러를 방지한다
	if (glewInit() != GLEW_OK)
	{
		SDL_Log("Failed to initialize GLEW");
		return false;
	}
	else
	{
		SDL_Log("TEST # glew Initialized");
	}
	// 일부 플랫폼에서 GLEW는 에러 코드를 내보내므로, 에러값을 제거하자
	glGetError();

	// 셰이더를 로드한다
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
	// Renderer 초기화
	mRenderer = SDL_CreateRenderer(mWindow,		// 렌더링을 위한 윈도우
								   -1,			// 어떤 그래픽 카드를 사용할지. 게임이 여러 윈도우를 사용할 때 의미가 생기는 값. 일반적으로 -1(기본 그카 사용)
								   SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC  // 초기화 플래그. 가속화 렌더러(그래픽 하드웨어 활용) |  수직동기화 활성화  => 이 함수의 유일한 플래그임
	);
	if (mRenderer == nullptr)
	{
		SDL_Log("Failed to create Renderer : %s", SDL_GetError());
		return false;
	}
	*/

	/*
	// 2D 그래픽(이미지)를 위한 SDL_Image 초기화
	if (IMG_Init(IMG_INIT_PNG) == 0) // PNG를 사용할 것임. BMP는 SDL이 SDL_image 없이도 지원하기 때문에 bmp 쓸거면 필요 없음. 0이면 초기화 실패
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
	

	// gl context 삭제
	SDL_GL_DeleteContext(mContext);

	// SDL_DestroyWindow로 SDL_Window 객체를 파괴
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
	* 그래픽스에서는 Color Buffer에 화면 모든 픽셀의 색상 정보를 저장해둔다 (x,y) 배열
	* 게임 루프의 "출력 생성" 단계 동안 매 프레임에서 게임은 그래픽 출력을 "Color Buffer"에 쓴다
	* 24비트 Color Depth = RGB 8비트씩. 여기에 알파 8비트 => 총 32비트.  1080p Color Buffer = 1920x1080x4byte = 7.9MB
	* "Frame Buffer"의 정확한 정의 = 색상 버퍼 + 기타 버퍼(깊이 버퍼 / 스텐실 버퍼) 의 조합
	*/

	//-----------------------------------------------------------------------------------------------------------------------------

	/* 2
	* 게임의 프레임속도와 화면의 프레임 갱신 속도가 다를 때 발생하는 Screen Tearing을 해결하려면 2가지의 변경이 필요하다
	* 1. 게임과 디스플레이가 공유하는 색상 버퍼를 2개로(후면, 전면) 두고, 교대로 사용한다 => Double Buffering
	*		=> 가장 처음에 게임은 후면 버퍼를 사용한다
	* 2. 만약 X버퍼를 디스플레이가 쓰고 있는데 게임이 Y버퍼에 다 쓰고 바로 X를 사용하려고 한다면? => 게임은 디스플레이가 버퍼X를 다 그릴 때까지 기다려야 한다
	*		=> 이를 "수직 동기화" / "Vertical Synchronization" / "vsync" 라 한다
	*
	* 참고로 최근 디스플레이는 Adaptive Refresh Rate(적응적 화면 재생 빈도) 라는 기술을 사용.
	* 디스플레이가 화면을 갱신할 때 게임에 통지하는 것이 아니라, 게임이 디스플레이에게 언제 화면을 갱신할지 알려주게 하는 기술
	*/

	//-----------------------------------------------------------------------------------------------------------------------------

	/* 3
	* 기본적으로 화면을 그리는 것은 다음 단계로 이루어진다 (매 프레임마다 작동)
	* 1. 후면 버퍼를 단색으로 클리어한다 (게임의 현재 버퍼) => 이 때 ZBuffer의 값들도 1.0으로 초기화해준다
	* 2. 전체 게임 장면을 그린다
	* 3. 전면 버퍼와 후면 버퍼를 교환한다
	*/

	// 색상을 회색으로 설정  1
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// 색상 버퍼, ZBuffer => (Frame Buffer) 초기화
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// GL_COLOR_BUFFER_BIT로 이 함수를 호출하면, 색상 버퍼를 지정된 색상으로 채운다

	// 장면을 그린다  2

	// 3D 렌더링에 있을 지도 모를 투명 물체를 제대로 렌더링 하기 위해(투명한 유리 뒤의 바위는 깊이 값이 더 크기 때문에, 가려진 부분이 그려지질 않는다)
	// 화가 알고리즘을 이용한다 
	// (바위를 먼저 그리고, 유리를 뒤에서부터 앞으로 렌더링. 픽셀이 렌더링 될 때마다 불투명한 오브젝트 너머에 있는 투명한 픽셀은 그려지지 않도록 각 픽셀의 깊이를 테스트)
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	mMeshShader->SetActive();
	mMeshShader->SetMatrixUniform("uViewProj", mView * mProjection);
	SetLightUniforms(mMeshShader);

	/*
	for (auto iter = mMeshShaders.begin(); iter != mMeshShaders.end(); iter++)
	{
		iter->second->SetActive();
		// 뷰-투영 행렬 갱신
		iter->second->SetMatrixUniform("uViewProj", mView * mProjection);
		// 라이트 설정
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

	// 알파 블랜딩을 위해서 옵션을 킨다 => 스프라이트 만을 위해 (이렇게 하면 2D 스프라이트는 3D 장면의 제일 앞에 나타난다 => 이를 게임 UI, HUD에 적용시키자)
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(
		GL_SRC_ALPHA,	// srcFactor = srcAlpha
		GL_ONE_MINUS_SRC_ALPHA	// dstFactor = 1 - srcAlpha
	);
	// 스프라이트 셰이더와 버텍스 배열 개체를 활성화
	mSpriteShader->SetActive();
	mSpriteVerts->SetActive();
	// 모든 스프라이트들을 그린다
	for (auto sprite : mSprites)
	{
		sprite->Draw(mSpriteShader);
	}

	// UIScreen는 Texture를 사용하므로, 스프라이트가 사용하는 같은 셰이더를 이용해서 UI를 그린다'
	for (auto ui : mGame->GetUIStack())
	{
		ui->Draw(mSpriteShader);
	}

	// 버퍼를 스왑해서 장면을 출력한다  3
	SDL_GL_SwapWindow(mWindow);		// 전면 버퍼와 후면 버퍼를 교체한다

	//SDL_SetRenderDrawColor(mRenderer, 220, 220, 220, 255); // 후면 버퍼를 지우기 위한 하나의 색 지정
	//SDL_RenderClear(mRenderer); // 1
	// 모든 스프라이트들을 그린다
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
	//const std::string& shaderName = mesh->GetMesh()->GetShaderName();	// -> 이 한 줄 때문에 모든 코드가 작동이 제대로 되지 않는다. 왜지?
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
	// 파일로부터 SDL_Surface 상으로 이미지 로드
	// 성공하면 SDL_Surface 포인터를 리턴, 실패하면 nullptr
	SDL_Surface* IMG_Load(
		const char* file // 이미지 파일 이름
		)

	// SDL_CreateTexutreFromSurface는 SDL_Surface를 SDL이 화면에 그리는 데 필요한 포맷인 SDL_Texture로 변환해준다
	// 성공하면 SDL_Texture 포인터 반환, 실패하면 nullptr
	SDL_Texture* SDL_CreateTextureFromSurface(
	SDL_Renderer * renderer,	// 사용할 렌더러
	SDL_Surface * surface	// 변환될 SDL_Surface
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
	// 먼저 화면공간좌표 x,y를 [-1,1] 범위값을 가지는 장치 좌표로 정규화시킨다
	Vector3 deviceCoord = screenPoint;
	deviceCoord.x /= mScreenWidth * 0.5f;	// OpenGL은 중앙이 (0,0) 오른쪽 위가 (512,384) 이므로 가로/세로 길이의 절반으로 나눠야 제대로 정규화가 된다
	deviceCoord.y /= mScreenHeight * 0.5f;

	// 이 때 하나의 x,y 좌표는 [0,1] 범위를 가지는 z값을 가진다(1이면 멀리 있는 평면의 점 = 카메라로 볼 수 있는 최대 거리)
	// 그러므로 언프로젝션을 제대로 수행하려면, [0,1] 범위의 z 요소값이 필요
	// normalized Device Coord = (ndcX, ndcY, z, 1)
	
	// 언프로젝션 행렬은 뷰-투영 행렬의 역행렬
	Matrix4 unprojection = mView * mProjection;
	unprojection.Invert();

	// 이 때, 언프로젝션 행렬을 NDC에 곱하면 w 요소값이 변경된다. 그래서 각 요솟값을 w로 나눠서 w요소를 1로 되돌리는 재정규화가 필요하다
	// temp = (ndc)(Unprojection)
	// worldPos = temp / temp.w

	// 언프로젝션 행렬과 w요소를 사용해서 위의 재졍규화를 수행한다
	return Vector3::TransformWithPerspDiv(deviceCoord, unprojection);
}

void Renderer::GetScreenDirection(Vector3& outStart, Vector3& outDir) const
{
	// 방향 벡터를 얻으려면, Unproject()를 두 번 수행한다
	// 시작점에서 한 번, 끝점에서 한 번
	// 그 후 벡터간에 뺄셈을 하고, 이 벡터를 정규화하면 방향 벡터를 얻을 수 있다

	// 시작점을 얻는다(가까운 평면(z = 0)의 화면상의 중심)
	Vector3 screenPoint(0.0f, 0.0f, 0.0f);
	outStart = Unproject(screenPoint);
	// 끝 점을 얻는다(가까운 평면과 먼 평면(z = 1) 사이에 있는 화면상의 중점)
	screenPoint.z = 0.9f;
	Vector3 end = Unproject(screenPoint);

	// 방향 벡터를 얻는다
	outDir = end - outStart;
	outDir.Normalize();
}

void Renderer::SetLightUniforms(Shader* shader)
{
	// 카메라 위치는 뷰 행렬의 역행렬로 얻을 수 있다
	Matrix4 invView = mView;
	invView.Invert();
	shader->SetVectorUniform("uCameraPos", invView.GetTranslation());	// 역행렬의 네 번째 행의 첫 3요소가 카메라의 세계 공간 위치에 해당한다

	// 주변광(Ambient)
	shader->SetVectorUniform("uAmbientLight", mAmbientLight);

	// 방향광
	shader->SetVectorUniform("uDirLight.mDirection", mDirLight.mDirection);
	shader->SetVectorUniform("uDirLight.mDiffuseColor", mDirLight.mDiffuseColor);
	shader->SetVectorUniform("uDirLight.mSpecColor", mDirLight.mSpecColor);

	// 점광
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

	// uniform 변수에 값을 전달하자
	// 뷰-투영 행렬은 프로그램에서 변경되지 않으므로 한 번만 설정 => 그래서 여기에서 설정함
	// 스프라이트 컴퓨넌트는 컴포넌트 소유자 액터의 세계 변환 행렬로 그려지므로, 세계 변환 행렬은 프레임마다 설정해야 한다 => spritecomponent의 draw에서 설정한다
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
	temp->SetActive();	// 지금은 모든 메시가 같은 셰이더를 사용한다고 가정. 각 메시마다 서로 다른 셰이더를 사용할 수도 있다
	mMeshShaders.emplace("Phong", temp);

	temp = new Shader();
	if (!temp->Load("Shaders/BasicMesh.vert", "Shaders/BasicMesh.frag"))
	{
		return false;
	}
	temp->SetActive();
	mMeshShaders.emplace("BasicMesh", temp);
	
	// 뷰, 투영 행렬 수정
	mView = Matrix4::CreateLookAt(
		Vector3::Zero,	// 카메라 위치
		Vector3::UnitX,	// 카메라가 바라보는 방향
		Vector3::UnitZ	// 카메라의 상향 벡터
	);
	mProjection = Matrix4::CreatePerspectiveFOV(
		Math::ToRadians(70.0f),	// 수평  FOV
		mScreenWidth,			// 뷰의 너비
		mScreenHeight,			// 뷰의 높이
		25.0f,					// 가까운 평면과의 거리
		10000.0f				// 먼 평면과의 거리
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

