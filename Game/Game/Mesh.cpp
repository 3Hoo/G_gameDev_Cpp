#include "Mesh.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Game.h"
#include "rapidjson/document.h"
#include "Math.h"
#include "Renderer.h"
#include <fstream>
#include <sstream>



Mesh::Mesh()
	: mRadius(0)
	, mVertexArray(nullptr)
	, mSpecPower(100.0f)
	, mBox(Vector3::Infinity, Vector3::NegInfinity)
{

}

Mesh::~Mesh()
{

}

bool Mesh::Load(const std::string& fileName, Renderer* renderer)
{
	// 파일 열기
	std::ifstream meshFile(fileName);
	if (!meshFile.is_open())
	{
		SDL_Log("File not found : Mesh %s", fileName.c_str());
		return false;
	}
	// 모든 텍스트를 읽어서 스트림으로 만든다
	std::stringstream filestream;
	filestream << meshFile.rdbuf();
	std::string content = filestream.str();
	rapidjson::StringStream jsonStr(content.c_str());
	rapidjson::Document document;

	// rapidJson으로 파일을 분석한다
	document.ParseStream(jsonStr);


	// rapidJson 오브젝트가 잘 생성이 되었는지 확인
	if (!document.IsObject())
	{
		SDL_Log("Mesh %s is not valid json", fileName.c_str());
		return false;
	}


	// 버전 체크하기
	int version = document["version"].GetInt();
	if (version != 1)
	{
		SDL_Log("Mesh %s not versino 1", fileName.c_str());
		return false;
	}


	// 셰이더 이름 가져오기
	mShaderName = document["shader"].GetString();
	SDL_Log("%s", mShaderName.c_str());


	// 텍스처 로드하기
	const rapidjson::Value& textures = document["textures"];
	if (!textures.IsArray() || textures.Size() < 1)
	{
		SDL_Log("Mesh %s has no textures, there should be at least one", fileName.c_str());
		return false;
	}
	for (rapidjson::SizeType i = 0; i < textures.Size(); i++)
	{
		// 텍스처가 렌더러에 이미 있는가?
		std::string texName = textures[i].GetString();
		Texture* t = renderer->GetTexture(texName);
		if (t == nullptr)
		{
			// 텍스처를 새로 로딩한다. 만약 위에서 nullptr이 되었다면 renderer의 gettexture() 함수는 새로 텍스처를 로딩했을 것이므로 다시 질의한다
			t = renderer->GetTexture(texName);
			if (t == nullptr)
			{
				// 만약 또 nullptr이라면 그냥 기본 이미지를 사용하자
				t = renderer->GetTexture("Assets/Default.png");
			}
		}
		mTextures.emplace_back(t);
	}


	// vertex format, shader 정보 가져오기
	size_t vertSize = 8;


	// vertices 로드하기
	const rapidjson::Value& vertsJson = document["vertices"];
	if (!vertsJson.IsArray() || vertsJson.Size() < 1)
	{
		SDL_Log("Mesh %s has no vertices", fileName.c_str());
	}

	std::vector<float> vertTemp;
	vertTemp.reserve(vertsJson.Size() * vertSize);
	mRadius = 0.0f;

	for (rapidjson::SizeType i = 0; i < vertsJson.Size(); i++)
	{
		// 지금은 8개의 요소가 있다고 생각한다
		const rapidjson::Value& vert = vertsJson[i];
		if (!vert.IsArray() || vert.Size() != 8)
		{
			SDL_Log("Unexpected vertex format for %s", fileName.c_str());
			return false;
		}

		Vector3 pos(vert[0].GetDouble(), vert[1].GetDouble(), vert[2].GetDouble());
		mRadius = Math::Max(mRadius, pos.LengthSq());
		mBox.UpdateMinMax(pos);

		// Add the floats
		for (rapidjson::SizeType i = 0; i < vert.Size(); i++)
		{
			vertTemp.emplace_back(static_cast<float>(vert[i].GetDouble()));
		}
	}

	mRadius = Math::Sqrt(mRadius);


	// indices 가져오기
	const rapidjson::Value& idxJson = document["indices"];
	if (!idxJson.IsArray() || idxJson.Size() < 1)
	{
		SDL_Log("Mesh %s has no indices", fileName.c_str());
		return false;
	}

	std::vector<unsigned int> idxTemp;
	idxTemp.reserve(idxJson.Size() * 3);

	for (rapidjson::SizeType i = 0; i < idxJson.Size(); i++)
	{
		const rapidjson::Value& idx = idxJson[i];
		if (!idx.IsArray() || idx.Size() != 3)
		{
			SDL_Log("Invalid indices for %s", fileName.c_str());
			return false;
		}

		idxTemp.emplace_back(idx[0].GetUint());
		idxTemp.emplace_back(idx[1].GetUint());
		idxTemp.emplace_back(idx[2].GetUint());
	}

	// specularPower 값 가져오기
	mSpecPower = static_cast<float>(document["specularPower"].GetDouble());

	
	// 버택스 배열 개체 생성
	mVertexArray = new VertexArray(vertTemp.data(), static_cast<unsigned>(vertTemp.size()) / vertSize, idxTemp.data(), static_cast<unsigned>(idxTemp.size()));

	return true;
}

void Mesh::Unload()
{
	delete mVertexArray;
	mVertexArray = nullptr;
}

Texture* Mesh::GetTexture(size_t index)
{
	if (index < mTextures.size())
	{
		return mTextures[index];
	}
	else
	{
		return nullptr;
	}
}